/*
    SPDX-FileCopyrightText: 2017 - 2019 Deepin Technology Co., Ltd.
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "chameleon.h"
#include "chameleonbutton.h"
#include "chameleonshadow.h"

#include <KDecoration3/DecoratedWindow>
#include <KDecoration3/DecorationButtonGroup>
#include <KDecoration3/DecorationSettings>

#include <KConfig>
#include <KConfigGroup>

#include <QDynamicPropertyChangeEvent>
#include <QFontMetricsF>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>

#include <algorithm>

// Set by gxde-kwin_x11 (DecoratedWindowImpl) on the decoration.
static const char s_noTitlebarProperty[] = "gxdeDeepinNoTitlebar";
static const char s_windowThemeProperty[] = "gxdeDeepinWindowTheme";
// Set by gxde-kwin_x11 (DecorationBridge) on the shared DecorationSettings.
static const char s_windowRadiusProperty[] = "gxdeDeepinWindowRadius";

static bool s_themeLoaded = false;
static qreal s_titlebarHeightOverride = 0;

// [deepin-chameleon] theme is written by the GXDE window manager D-Bus service
// (com.deepin.wm) when the user switches between the light and dark theme.
static void reloadThemeFromConfig()
{
    const KConfig config(QStringLiteral(CHAMELEON_KWIN_CONFIG), KConfig::CascadeConfig);
    const KConfigGroup group(&config, QStringLiteral("deepin-chameleon"));

    const QString theme = group.readEntry("theme", QString());
    if (theme.isEmpty() || !ChameleonTheme::instance()->setTheme(theme)) {
        ChameleonTheme::instance()->setTheme(ChameleonTheme::Light, QStringLiteral("deepin"));
    }

    const qreal height = group.readEntry("titlebarHeight", 0.0);
    s_titlebarHeightOverride = (height >= 24 && height <= 50) ? height : 0;

    s_themeLoaded = true;
}

Chameleon::Chameleon(QObject *parent, const QVariantList &args)
    : KDecoration3::Decoration(parent, args)
{
}

Chameleon::~Chameleon() = default;

bool Chameleon::init()
{
    if (!s_themeLoaded) {
        reloadThemeFromConfig();
    }

    m_noTitleBar = property(s_noTitlebarProperty).toBool();
    m_windowTheme = property(s_windowThemeProperty).toMap();

    KDecoration3::DecoratedWindow *w = window();
    auto s = settings();

    m_leftButtons = new KDecoration3::DecorationButtonGroup(KDecoration3::DecorationButtonGroup::Position::Left, this, &ChameleonButton::create);
    m_rightButtons = new KDecoration3::DecorationButtonGroup(KDecoration3::DecorationButtonGroup::Position::Right, this, &ChameleonButton::create);
    connect(m_leftButtons, &KDecoration3::DecorationButtonGroup::geometryChanged, this, &Chameleon::positionButtons);
    connect(m_rightButtons, &KDecoration3::DecorationButtonGroup::geometryChanged, this, &Chameleon::positionButtons);

    connect(w, &KDecoration3::DecoratedWindow::activeChanged, this, &Chameleon::updateActive);
    connect(w, &KDecoration3::DecoratedWindow::captionChanged, this, &Chameleon::updateTitleGeometry);
    connect(w, &KDecoration3::DecoratedWindow::sizeChanged, this, &Chameleon::updateTitleBarArea);
    connect(w, &KDecoration3::DecoratedWindow::maximizedChanged, this, [this]() {
        updateTitleBarArea();
        updateResizeOnlyBorders();
        updateShadow();
    });
    connect(w, &KDecoration3::DecoratedWindow::adjacentScreenEdgesChanged, this, [this]() {
        updateBorderGeometry();
        updateShadow();
        update();
    });
    connect(w, &KDecoration3::DecoratedWindow::iconChanged, this, [this]() {
        update();
    });
    connect(w, &KDecoration3::DecoratedWindow::paletteChanged, this, [this]() {
        update();
    });

    connect(s.get(), &KDecoration3::DecorationSettings::alphaChannelSupportedChanged, this, [this]() {
        updateTitleBarArea();
        updateShadow();
    });
    connect(s.get(), &KDecoration3::DecorationSettings::fontChanged, this, &Chameleon::updateTitleGeometry);
    // The button groups recreate their buttons on these changes.
    connect(s.get(), &KDecoration3::DecorationSettings::decorationButtonsLeftChanged, this, &Chameleon::updateButtonsGeometry, Qt::QueuedConnection);
    connect(s.get(), &KDecoration3::DecorationSettings::decorationButtonsRightChanged, this, &Chameleon::updateButtonsGeometry, Qt::QueuedConnection);
    connect(s.get(), &KDecoration3::DecorationSettings::reconfigured, this, [this]() {
        reloadThemeFromConfig();
        updateTheme();
    });
    // For gxdeDeepinWindowRadius.
    s->installEventFilter(this);

    m_initialized = true;
    updateTheme();
    return true;
}

bool Chameleon::event(QEvent *event)
{
    if (event->type() == QEvent::DynamicPropertyChange) {
        const QByteArray name = static_cast<QDynamicPropertyChangeEvent *>(event)->propertyName();
        if (name == s_noTitlebarProperty) {
            const bool noTitleBar = property(s_noTitlebarProperty).toBool();
            if (noTitleBar != m_noTitleBar) {
                m_noTitleBar = noTitleBar;
                if (m_initialized) {
                    updateTitleBarArea();
                }
            }
            return true;
        }
        if (name == s_windowThemeProperty) {
            m_windowTheme = property(s_windowThemeProperty).toMap();
            if (m_initialized) {
                updateTheme();
            }
            return true;
        }
    }
    return KDecoration3::Decoration::event(event);
}

bool Chameleon::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::DynamicPropertyChange && watched == settings().get()
        && static_cast<QDynamicPropertyChangeEvent *>(event)->propertyName() == s_windowRadiusProperty) {
        updateBorderGeometry();
        updateShadow();
        update();
    }
    return KDecoration3::Decoration::eventFilter(watched, event);
}

UIWindowType Chameleon::windowType() const
{
    // The decoration is parented to the KWin::Window it decorates.
    if (parent() && parent()->property("dialog").toBool()) {
        return UIWindowType::Dialog;
    }
    return UIWindowType::Normal;
}

qreal Chameleon::scale() const
{
    // gxde-kwin_x11 runs without Qt high DPI scaling, so this follows Xft.dpi
    // like DTK applications do. The theme files are written for 96 dpi.
    const QScreen *screen = QGuiApplication::primaryScreen();
    return screen ? std::max(1.0, screen->logicalDotsPerInch() / 96.0) : 1.0;
}

bool Chameleon::noTitleBar() const
{
    return m_noTitleBar;
}

qreal Chameleon::titleBarHeight() const
{
    const qreal height = s_titlebarHeightOverride > 0 ? s_titlebarHeightOverride : m_config.titlebarConfig.height;
    return std::round(height * scale());
}

qreal Chameleon::borderWidth() const
{
    return windowNeedBorder() ? m_config.borderConfig.borderWidth : 0;
}

qreal Chameleon::windowRadius() const
{
    // Rounded corners need compositing, and maximized or tiled windows are
    // square, as in deepin-kwin.
    if (!settings()->isAlphaChannelSupported() || window()->isMaximized()
        || window()->adjacentScreenEdges() != Qt::Edges()) {
        return 0;
    }

    // Window values from dxcb are in device pixels already.
    const QVariant own = m_windowTheme.value(QStringLiteral("windowRadius"));
    if (own.isValid()) {
        return std::max(0.0, ChameleonTheme::takePos(own, QPointF()).x());
    }

    bool ok = false;
    const qreal desktop = settings()->property(s_windowRadiusProperty).toReal(&ok);
    if (ok) {
        return std::max(0.0, std::round(desktop * scale()));
    }

    return std::max(0.0, m_config.radius.x());
}

bool Chameleon::windowNeedBorder() const
{
    if (window()->isMaximized()) {
        return false;
    }
    // While compositing the border is an outline drawn by KWin.
    return !settings()->isAlphaChannelSupported();
}

QColor Chameleon::textColor() const
{
    if (m_config.titlebarConfig.font.textColor.isValid()) {
        return m_config.titlebarConfig.font.textColor;
    }
    const auto *w = window();
    return w->color(w->isActive() ? KDecoration3::ColorGroup::Active : KDecoration3::ColorGroup::Inactive,
                    KDecoration3::ColorRole::Foreground);
}

QColor Chameleon::backgroundColor() const
{
    if (m_config.titlebarConfig.backgroundColor.isValid()) {
        return m_config.titlebarConfig.backgroundColor;
    }
    const auto *w = window();
    return w->color(w->isActive() ? KDecoration3::ColorGroup::Active : KDecoration3::ColorGroup::Inactive,
                    KDecoration3::ColorRole::TitleBar);
}

QIcon Chameleon::minimizeIcon() const
{
    return m_config.titlebarConfig.minimizeBtn.btnIcon;
}

QIcon Chameleon::maximizeIcon() const
{
    return m_config.titlebarConfig.maximizeBtn.btnIcon;
}

QIcon Chameleon::unmaximizeIcon() const
{
    return m_config.titlebarConfig.unmaximizeBtn.btnIcon;
}

QIcon Chameleon::closeIcon() const
{
    return m_config.titlebarConfig.closeBtn.btnIcon;
}

qreal Chameleon::menuIconSize() const
{
    const auto &tb = m_config.titlebarConfig;
    const qreal ratio = tb.height > 0 ? titleBarHeight() / tb.height : 1.0;
    return std::min(tb.menuBtn.width, tb.menuBtn.height) * ratio;
}

void Chameleon::updateTheme()
{
    const QString ownTheme = QString::fromUtf8(m_windowTheme.value(QStringLiteral("theme")).toByteArray());
    const ChameleonTheme::ConfigGroup *group = ownTheme.isEmpty()
        ? ChameleonTheme::instance()->themeConfig(windowType())
        : ChameleonTheme::instance()->themeConfig(ownTheme, windowType());
    if (group) {
        m_configGroup = *group;
    }
    updateActive();
}

void Chameleon::applyWindowTheme(ChameleonTheme::ThemeConfig &config) const
{
    const auto value = [this](const char *key) {
        return m_windowTheme.value(QLatin1String(key));
    };

    if (const QVariant v = value("borderWidth"); v.canConvert<int>() && v.isValid()) {
        config.borderConfig.borderWidth = std::max(0, v.toInt());
    }
    if (const QColor c = value("borderColor").value<QColor>(); c.isValid()) {
        config.borderConfig.borderColor = c;
    }
    if (const QVariant v = value("shadowRadius"); v.isValid()) {
        config.shadowConfig.shadowRadius = std::max(0, v.toInt());
    }
    if (const QVariant v = value("shadowOffset"); v.isValid()) {
        config.shadowConfig.shadowOffset = ChameleonTheme::takePos(v, config.shadowConfig.shadowOffset);
    }
    if (const QColor c = value("shadowColor").value<QColor>(); c.isValid()) {
        config.shadowConfig.shadowColor = c;
    }
    if (const QVariant v = value("mouseInputAreaMargins"); v.isValid()) {
        config.mouseInputAreaMargins = ChameleonTheme::takeMargins(v, config.mouseInputAreaMargins);
    }
}

void Chameleon::updateActive()
{
    m_config = window()->isActive() ? m_configGroup.normal : m_configGroup.inactive;
    applyWindowTheme(m_config);

    updateResizeOnlyBorders();
    updateTitleBarArea();
    updateShadow();
    update();
}

void Chameleon::updateTitleBarArea()
{
    const qreal border = borderWidth();
    const qreal height = noTitleBar() ? 0 : titleBarHeight();

    // Only title bars at the top are supported; every shipped theme uses it.
    setBorders(QMarginsF(border, border + height, border, border));
    setTitleBar(QRectF(border, border, window()->width(), height));

    updateBorderGeometry();
    updateButtonsGeometry();
}

void Chameleon::updateBorderGeometry()
{
    const qreal radius = windowRadius();

    // KWin clips the client with this radius. Below a title bar the top
    // corners of the client are not window corners.
    const qreal top = noTitleBar() ? radius : 0;
    setBorderRadius(KDecoration3::BorderRadius(top, top, radius, radius));

    const bool outline = settings()->isAlphaChannelSupported() && !window()->isMaximized()
        && m_config.borderConfig.borderWidth > 0 && m_config.borderConfig.borderColor.alpha() > 0;
    if (outline) {
        // KWin's border shader blends the color as premultiplied alpha.
        const QColor c = m_config.borderConfig.borderColor;
        const QColor premultiplied = QColor::fromRgbF(c.redF() * c.alphaF(), c.greenF() * c.alphaF(), c.blueF() * c.alphaF(), c.alphaF());
        setBorderOutline(KDecoration3::BorderOutline(m_config.borderConfig.borderWidth, premultiplied,
                                                     KDecoration3::BorderRadius(radius)));
    } else {
        setBorderOutline(KDecoration3::BorderOutline());
    }
}

void Chameleon::updateButtonsGeometry()
{
    if (!m_leftButtons || !m_rightButtons) {
        return;
    }

    const qreal height = noTitleBar() ? 0 : titleBarHeight();
    const qreal ratio = m_config.titlebarConfig.height > 0 ? height / m_config.titlebarConfig.height : 1.0;
    const qreal buttonWidth = std::round(m_config.titlebarConfig.closeBtn.width * ratio);

    m_leftButtons->setSpacing(0);
    m_rightButtons->setSpacing(0);

    const auto buttons = m_leftButtons->buttons() + m_rightButtons->buttons();
    for (KDecoration3::DecorationButton *button : buttons) {
        // The window icon sits in a square button.
        const qreal width = button->type() == KDecoration3::DecorationButtonType::Menu ? height : buttonWidth;
        button->setGeometry(QRectF(QPointF(0, 0), QSizeF(width, height)));
    }

    positionButtons();
}

void Chameleon::positionButtons()
{
    if (!m_leftButtons || !m_rightButtons) {
        return;
    }

    const QRectF bar = titleBar();
    const qreal hPadding = window()->isMaximizedHorizontally() ? 0 : settings()->smallSpacing();

    m_leftButtons->setPos(QPointF(bar.left() + hPadding, bar.top()));
    m_rightButtons->setPos(QPointF(bar.right() - m_rightButtons->geometry().width(), bar.top()));

    updateTitleGeometry();
}

void Chameleon::updateTitleGeometry()
{
    if (!m_leftButtons || !m_rightButtons) {
        return;
    }

    const QRectF bar = titleBar();
    const qreal spacing = settings()->smallSpacing();
    const QFontMetricsF metrics(settings()->font());

    m_title = window()->caption();

    QRectF available = bar;
    if (!m_leftButtons->buttons().isEmpty()) {
        available.setLeft(m_leftButtons->geometry().right() + spacing);
    }
    if (!m_rightButtons->buttons().isEmpty()) {
        available.setRight(m_rightButtons->geometry().left() - spacing);
    }

    const qreal fullWidth = metrics.horizontalAdvance(m_title);
    if (available.width() <= 0) {
        m_titleArea = QRectF();
        m_title.clear();
    } else if (m_config.titlebarConfig.font.textAlign != QLatin1String("left")
               && fullWidth <= 2 * std::min(bar.center().x() - available.left(), available.right() - bar.center().x())) {
        // Centred on the whole title bar when it does not collide with the buttons.
        m_titleArea = QRectF(0, bar.top(), fullWidth, bar.height());
        m_titleArea.moveCenter(bar.center());
    } else if (fullWidth > available.width()) {
        m_title = metrics.elidedText(m_title, Qt::ElideRight, available.width());
        m_titleArea = available;
    } else if (m_config.titlebarConfig.font.textAlign == QLatin1String("left")) {
        m_titleArea = QRectF(available.left(), bar.top(), fullWidth, bar.height());
    } else {
        // Centred in the space between the buttons.
        m_titleArea = QRectF(0, bar.top(), fullWidth, bar.height());
        m_titleArea.moveCenter(available.center());
    }

    update();
}

void Chameleon::updateShadow()
{
    if (!settings()->isAlphaChannelSupported() || window()->isMaximized()) {
        setShadow(nullptr);
        return;
    }

    ChameleonTheme::ThemeConfig config = m_config;
    const qreal radius = windowRadius();
    config.radius = QPointF(radius, radius);
    // The border is the outline of the decoration.
    setShadow(ChameleonShadow::instance()->getShadow(config, false));
}

void Chameleon::updateResizeOnlyBorders()
{
    if (window()->isMaximized()) {
        setResizeOnlyBorders(QMarginsF());
    } else {
        setResizeOnlyBorders(m_config.mouseInputAreaMargins);
    }
}

void Chameleon::paint(QPainter *painter, const QRectF &repaintArea)
{
    const QRectF frame(QPointF(0, 0), size());

    const qreal border = borderWidth();
    if (border > 0) {
        // The client covers the inner part.
        painter->fillRect(frame & repaintArea, m_config.borderConfig.borderColor);
    }

    if (noTitleBar()) {
        return;
    }

    painter->save();
    if (const qreal radius = windowRadius(); radius > 0) {
        QPainterPath path;
        path.addRoundedRect(frame, radius, radius);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setClipPath(path);
    }
    painter->fillRect(titleBar() & repaintArea, backgroundColor());
    painter->restore();

    if (!m_title.isEmpty() && m_titleArea.intersects(repaintArea)) {
        painter->setFont(settings()->font());
        painter->setPen(textColor());
        painter->drawText(m_titleArea, Qt::AlignCenter | Qt::TextSingleLine, m_title);
    }

    m_leftButtons->paint(painter, repaintArea);
    m_rightButtons->paint(painter, repaintArea);
}

#include "moc_chameleon.cpp"
