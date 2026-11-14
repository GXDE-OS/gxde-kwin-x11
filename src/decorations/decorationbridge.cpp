/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "decorationbridge.h"

#include "config-kwin.h"

#include "atoms.h"
#include "decoratedwindow.h"
#include "decorations_logging.h"
#include "gxdexsettings.h"
#include "main.h"
#include "netinfo.h"
#include "settings.h"
// KWin core
#include "wayland/server_decoration.h"
#include "wayland_server.h"
#include "window.h"
#include "workspace.h"
#include "x11window.h"

// KDecoration
#include <KDecoration3/DecoratedWindow>
#include <KDecoration3/Decoration>
#include <KDecoration3/DecorationSettings>

// Frameworks
#include <KPluginFactory>
#include <KPluginMetaData>

// Qt
#include <QMetaProperty>
#include <QPainter>

namespace KWin
{
namespace Decoration
{

static const QString s_aurorae = QStringLiteral("org.kde.kwin.aurorae");
static const QString s_pluginName = QStringLiteral("org.kde.kdecoration3");
// Decoration plugins built against GXDE's KDecoration 3 live below the
// gxde-kwin_x11 plugin directory. Plugins in the shared namespace are linked
// against the upstream library and cannot be loaded into this process.
static const QString s_pluginNamespace = KWIN_PLUGINDIR + QLatin1Char('/') + s_pluginName;
static const QString s_configKeyName = QStringLiteral("org.kde.kdecoration2");
static const QString s_defaultPlugin = QStringLiteral("com.deepin.chameleon");

static void migrateAuroraeTheme()
{
    const QString themeName = kwinApp()->config()->group(s_configKeyName).readEntry("theme");
    if (!themeName.startsWith(QLatin1String("__aurorae__svg__"))) {
        return;
    }

    const QString pluginName = kwinApp()->config()->group(s_configKeyName).readEntry("library");
    if (pluginName != QLatin1String("org.kde.kwin.aurorae")) {
        return;
    }

    kwinApp()->config()->group(s_configKeyName).writeEntry("library", "org.kde.kwin.aurorae.v2");
}

DecorationBridge::DecorationBridge()
    : m_factory(nullptr)
    , m_showToolTips(false)
    , m_settings()
    , m_noPlugin(false)
{
    migrateAuroraeTheme();
    readDecorationOptions();

    if (kwinApp()->x11Connection()) {
        m_xsettings = std::make_unique<GxdeXSettingsWatcher>();
        connect(m_xsettings.get(), &GxdeXSettingsWatcher::changed, this, &DecorationBridge::updateGxdeSettings);
    }
}

DecorationBridge::~DecorationBridge() = default;

void DecorationBridge::updateGxdeSettings()
{
    if (!m_settings) {
        return;
    }
    // Window corner radius of the desktop in logical pixels (dtk-window-radius
    // of com.deepin.xsettings), invalid if the settings daemon does not set it.
    m_settings->setProperty("gxdeDeepinWindowRadius", m_xsettings ? m_xsettings->value(QByteArrayLiteral("DTK/WindowRadius")) : QVariant());
}

QString DecorationBridge::readPlugin()
{
    return kwinApp()->config()->group(s_configKeyName).readEntry("library", s_defaultPlugin);
}

static bool readNoPlugin()
{
    return kwinApp()->config()->group(s_configKeyName).readEntry("NoPlugin", false);
}

QString DecorationBridge::readTheme() const
{
    return kwinApp()->config()->group(s_configKeyName).readEntry("theme", m_defaultTheme);
}

void DecorationBridge::readDecorationOptions()
{
    m_showToolTips = kwinApp()->config()->group(s_configKeyName).readEntry("ShowToolTips", true);
}

bool DecorationBridge::hasPlugin()
{
    const DecorationBridge *bridge = workspace()->decorationBridge();
    if (!bridge) {
        return false;
    }
    return !bridge->m_noPlugin && bridge->m_factory;
}

void DecorationBridge::init()
{
    m_noPlugin = readNoPlugin();
    if (m_noPlugin) {
        if (waylandServer()) {
            waylandServer()->decorationManager()->setDefaultMode(ServerSideDecorationManagerInterface::Mode::None);
        }
        updateDeepinSupport();
        return;
    }
    m_plugin = readPlugin();
    m_settings = std::make_shared<KDecoration3::DecorationSettings>(this);
    updateGxdeSettings();
    if (!initPlugin()) {
        if (m_plugin != s_defaultPlugin) {
            // try loading default plugin
            m_plugin = s_defaultPlugin;
            initPlugin();
        }
        // default plugin failed to load, try fallback
        if (!m_factory) {
            m_plugin = s_aurorae;
            initPlugin();
        }
    }
    if (waylandServer()) {
        waylandServer()->decorationManager()->setDefaultMode(m_factory ? ServerSideDecorationManagerInterface::Mode::Server : ServerSideDecorationManagerInterface::Mode::None);
    }
    updateDeepinSupport();
}

void DecorationBridge::updateDeepinSupport()
{
    // Announce the Deepin decoration extensions only while the loaded plugin
    // handles them. dxcb switches DTK windows to its no-titlebar mode as soon
    // as _DEEPIN_NO_TITLEBAR is in _NET_SUPPORTED, so a plugin that ignores it
    // would draw a second title bar above the one of the application.
    const bool supported = !m_noPlugin && m_factory && m_deepinNoTitlebarSupported;
    qCDebug(KWIN_DECORATIONS) << "Deepin decoration extensions supported:" << supported << "plugin:" << m_plugin;
    if (!kwinApp()->x11Connection() || supported == m_deepinSupportAnnounced) {
        return;
    }
    m_deepinSupportAnnounced = supported;

    RootInfo::setExtraSupported(atoms->deepin_chameleon, supported);
    RootInfo::setExtraSupported(atoms->deepin_no_titlebar, supported);
    RootInfo::setExtraSupported(atoms->deepin_force_decorate, supported);

    for (Window *window : workspace()->windows()) {
        if (auto x11Window = qobject_cast<X11Window *>(window)) {
            x11Window->updateDeepinForceDecorate();
        }
    }
}

bool DecorationBridge::initPlugin()
{
    const KPluginMetaData metaData = KPluginMetaData::findPluginById(s_pluginNamespace, m_plugin);
    if (!metaData.isValid()) {
        qCWarning(KWIN_DECORATIONS) << "Could not locate decoration plugin" << m_plugin;
        return false;
    }
    qCDebug(KWIN_DECORATIONS) << "Trying to load decoration plugin: " << metaData.fileName();
    if (auto factoryResult = KPluginFactory::loadFactory(metaData)) {
        m_factory.reset(factoryResult.plugin);
        loadMetaData(metaData.rawData());
        return true;
    } else {
        qCWarning(KWIN_DECORATIONS) << "Error loading plugin:" << factoryResult.errorText;
        return false;
    }
}

static void recreateDecorations()
{
    Workspace::self()->forEachWindow([](Window *window) {
        window->invalidateDecoration();
    });
}

void DecorationBridge::reconfigure()
{
    readDecorationOptions();

    if (m_noPlugin != readNoPlugin()) {
        m_noPlugin = !m_noPlugin;
        // no plugin setting changed
        if (m_noPlugin) {
            // decorations disabled now
            m_plugin = QString();
            m_factory.reset();
            m_settings.reset();
            updateDeepinSupport();
        } else {
            // decorations enabled now
            init();
        }
        recreateDecorations();
        return;
    }

    const QString newPlugin = readPlugin();
    if (newPlugin != m_plugin) {
        // plugin changed, recreate everything
        auto oldFactory = std::move(m_factory);
        const auto oldPluginName = m_plugin;
        m_plugin = newPlugin;
        if (!initPlugin()) {
            // loading new plugin failed
            m_factory = std::move(oldFactory);
            m_plugin = oldPluginName;
        } else {
            updateDeepinSupport();
            recreateDecorations();
            // TODO: unload and destroy old plugin
        }
    } else {
        // same plugin, but theme might have changed
        const QString oldTheme = m_theme;
        m_theme = readTheme();
        if (m_theme != oldTheme) {
            recreateDecorations();
        }
    }
}

void DecorationBridge::loadMetaData(const QJsonObject &object)
{
    // reset all settings
    m_recommendedBorderSize = QString();
    m_theme = QString();
    m_defaultTheme = QString();
    m_deepinNoTitlebarSupported = false;

    // load the settings
    const QJsonValue decoSettings = object.value(s_pluginName);
    if (decoSettings.isUndefined()) {
        // no settings
        return;
    }
    const QVariantMap decoSettingsMap = decoSettings.toObject().toVariantMap();
    m_deepinNoTitlebarSupported = decoSettingsMap.value(QStringLiteral("gxdeDeepinNoTitlebar")).toBool();
    auto recBorderSizeIt = decoSettingsMap.find(QStringLiteral("recommendedBorderSize"));
    if (recBorderSizeIt != decoSettingsMap.end()) {
        m_recommendedBorderSize = recBorderSizeIt.value().toString();
    }
    findTheme(decoSettingsMap);

    Q_EMIT metaDataLoaded();
}

void DecorationBridge::findTheme(const QVariantMap &map)
{
    auto it = map.find(QStringLiteral("themes"));
    if (it == map.end()) {
        return;
    }
    if (!it.value().toBool()) {
        return;
    }
    it = map.find(QStringLiteral("defaultTheme"));
    m_defaultTheme = it != map.end() ? it.value().toString() : QString();
    m_theme = readTheme();
}

std::unique_ptr<KDecoration3::DecoratedWindowPrivate> DecorationBridge::createClient(KDecoration3::DecoratedWindow *client, KDecoration3::Decoration *decoration)
{
    return std::unique_ptr<DecoratedWindowImpl>(new DecoratedWindowImpl(static_cast<Window *>(decoration->parent()), client, decoration));
}

std::unique_ptr<KDecoration3::DecorationSettingsPrivate> DecorationBridge::settings(KDecoration3::DecorationSettings *parent)
{
    return std::unique_ptr<SettingsImpl>(new SettingsImpl(parent));
}

KDecoration3::Decoration *DecorationBridge::createDecoration(Window *window)
{
    if (m_noPlugin) {
        return nullptr;
    }
    if (!m_factory) {
        return nullptr;
    }
    QVariantMap args({{QStringLiteral("bridge"), QVariant::fromValue(this)}});

    if (!m_theme.isEmpty()) {
        args.insert(QStringLiteral("theme"), m_theme);
    }
    auto deco = m_factory->create<KDecoration3::Decoration>(window, QVariantList{args});
    deco->setSettings(m_settings);
    deco->create();
    deco->init();
    return deco;
}

static QString settingsProperty(const QVariant &variant)
{
    if (QLatin1String(variant.typeName()) == QLatin1String("KDecoration3::BorderSize")) {
        return QString::number(variant.toInt());
    } else if (QLatin1String(variant.typeName()) == QLatin1String("QList<KDecoration3::DecorationButtonType>")) {
        const auto &b = variant.value<QList<KDecoration3::DecorationButtonType>>();
        QString buffer;
        for (auto it = b.begin(); it != b.end(); ++it) {
            if (it != b.begin()) {
                buffer.append(QStringLiteral(", "));
            }
            buffer.append(QString::number(int(*it)));
        }
        return buffer;
    }
    return variant.toString();
}

QString DecorationBridge::supportInformation() const
{
    QString b;
    if (m_noPlugin) {
        b.append(QStringLiteral("Decorations are disabled"));
    } else {
        b.append(QStringLiteral("Plugin: %1\n").arg(m_plugin));
        b.append(QStringLiteral("Theme: %1\n").arg(m_theme));
        b.append(QStringLiteral("Plugin recommends border size: %1\n").arg(m_recommendedBorderSize.isNull() ? "No" : m_recommendedBorderSize));
        const QMetaObject *metaOptions = m_settings->metaObject();
        for (int i = 0; i < metaOptions->propertyCount(); ++i) {
            const QMetaProperty property = metaOptions->property(i);
            if (QLatin1String(property.name()) == QLatin1String("objectName")) {
                continue;
            }
            b.append(QStringLiteral("%1: %2\n").arg(property.name(), settingsProperty(m_settings->property(property.name()))));
        }
    }
    return b;
}

} // Decoration
} // KWin

#include "moc_decorationbridge.cpp"
