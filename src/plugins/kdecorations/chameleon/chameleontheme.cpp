/*
    SPDX-FileCopyrightText: 2017 - 2019 Deepin Technology Co., Ltd.
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "chameleontheme.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

Q_LOGGING_CATEGORY(CHAMELEON, "gxde.kwin.chameleon", QtWarningMsg)

static const QString s_baseTheme = QStringLiteral("deepin");
static const QString s_baseThemeDir = QStringLiteral(":/deepin/themes");

QPair<qreal, qreal> ChameleonTheme::takePair(const QVariant &value, const QPair<qreal, qreal> defaultValue)
{
    if (!value.isValid()) {
        return defaultValue;
    }

    const QStringList l = value.toString().split(QLatin1Char(','));
    if (l.count() < 2) {
        return defaultValue;
    }

    return qMakePair(l.at(0).toDouble(), l.at(1).toDouble());
}

QMarginsF ChameleonTheme::takeMargins(const QVariant &value, const QMarginsF &defaultValue)
{
    if (!value.isValid()) {
        return defaultValue;
    }

    QStringList l = value.toStringList();
    if (l.isEmpty()) {
        l = value.toString().split(QLatin1Char(','));
    }
    if (l.count() < 4) {
        return defaultValue;
    }

    return QMarginsF(l.at(0).toDouble(), l.at(1).toDouble(), l.at(2).toDouble(), l.at(3).toDouble());
}

QPointF ChameleonTheme::takePos(const QVariant &value, const QPointF defaultValue)
{
    const auto pair = takePair(value, qMakePair(defaultValue.x(), defaultValue.y()));
    return QPointF(pair.first, pair.second);
}

static QColor takeColor(const QVariant &value, const QColor &defaultValue)
{
    const QColor c(value.toString());
    return c.isValid() ? c : defaultValue;
}

static QIcon takeIcon(const QJsonObject &iconObj)
{
    QIcon icon;
    icon.addFile(iconObj.value(QLatin1String("normal")).toString());
    icon.addFile(iconObj.value(QLatin1String("hover")).toString(), QSize(), QIcon::Active);
    icon.addFile(iconObj.value(QLatin1String("press")).toString(), QSize(), QIcon::Selected);
    icon.addFile(iconObj.value(QLatin1String("disabled")).toString(), QSize(), QIcon::Disabled);
    return icon;
}

class GlobalChameleonTheme : public ChameleonTheme
{
};
Q_GLOBAL_STATIC(GlobalChameleonTheme, s_globalTheme)

ChameleonTheme *ChameleonTheme::instance()
{
    return s_globalTheme;
}

/*
 * A theme value is either a plain value or an object keyed by window state:
 *
 *   "shadowColor": { "active": "#FF0000", "inactive": "#00FF00", "unmanaged": "#0000FF" }
 *   "shadowColor": { "managed": "#FF0000", "unmanaged": "#0000FF" }
 */
static QVariant parseAttribute(const QJsonObject &jsonObj, const QString &key, const QString &status)
{
    const QJsonValue val = jsonObj.value(key);
    if (val.isUndefined() || val.isNull()) {
        return QVariant();
    }
    if (!val.isObject()) {
        return val.toVariant();
    }

    QStringList keys;
    if (status == QLatin1String("inactive")) {
        keys = {QStringLiteral("managed"), QStringLiteral("inactive")};
    } else if (status == QLatin1String("unmanaged")) {
        keys = {QStringLiteral("unmanaged")};
    } else {
        keys = {QStringLiteral("managed"), QStringLiteral("active")};
    }

    const QJsonObject valObj = val.toObject();
    for (auto it = valObj.begin(); it != valObj.end(); ++it) {
        if (keys.contains(it.key()) && !it.value().isUndefined() && !it.value().isNull()) {
            return it.value().toVariant();
        }
    }
    return QVariant();
}

static void parse(const QJsonObject &obj, const QString &key, const QString &status, qreal &config, qreal defaultValue)
{
    const QVariant result = parseAttribute(obj, key, status);
    config = result.isValid() ? result.toDouble() : defaultValue;
}

static void parse(const QJsonObject &obj, const QString &key, const QString &status, int &config, int defaultValue)
{
    const QVariant result = parseAttribute(obj, key, status);
    config = result.isValid() ? result.toInt() : defaultValue;
}

static void parse(const QJsonObject &obj, const QString &key, const QString &status, QString &config, const QString &defaultValue)
{
    const QVariant result = parseAttribute(obj, key, status);
    config = result.isValid() ? result.toString() : defaultValue;
}

static void parse(const QJsonObject &obj, const QString &key, const QString &status, QColor &config, const QColor &defaultValue)
{
    config = takeColor(parseAttribute(obj, key, status), defaultValue);
}

static void parse(const QJsonObject &obj, const QString &key, const QString &status, Qt::Edge &config, Qt::Edge defaultValue)
{
    const QVariant result = parseAttribute(obj, key, status);
    config = result.isValid() ? static_cast<Qt::Edge>(result.toInt()) : defaultValue;
}

static void parse(const QJsonObject &obj, const QString &key, const QString &status, QPointF &config, const QPointF &defaultValue)
{
    config = ChameleonTheme::takePos(parseAttribute(obj, key, status), defaultValue);
}

static void parse(const QJsonObject &obj, const QString &key, const QString &status, QMarginsF &config, const QMarginsF &defaultValue)
{
    config = ChameleonTheme::takeMargins(parseAttribute(obj, key, status), defaultValue);
}

static void parseWindowDecoration(const QJsonObject &windowDecObj, const QString &status,
                                  ChameleonTheme::ThemeConfig *config, const ChameleonTheme::ThemeConfig *base)
{
    using ThemeConfig = ChameleonTheme::ThemeConfig;
    static const ThemeConfig defaults;
    if (!base) {
        base = &defaults;
    }

    config->windowDesc = windowDecObj.value(QLatin1String("desc")).toString();

    const QJsonObject titlebarObj = windowDecObj.value(QLatin1String("titlebar")).toObject();
    // Unmanaged windows have no title bar.
    if (status != QLatin1String("unmanaged") && !titlebarObj.isEmpty()) {
        auto &tb = config->titlebarConfig;
        const auto &baseTb = base->titlebarConfig;
        parse(titlebarObj, QStringLiteral("height"), status, tb.height, baseTb.height);
        parse(titlebarObj, QStringLiteral("area"), status, tb.area, baseTb.area);
        parse(titlebarObj, QStringLiteral("bgcolor"), status, tb.backgroundColor, baseTb.backgroundColor);
        parse(titlebarObj, QStringLiteral("font-family"), status, tb.font.fontFamily, baseTb.font.fontFamily);
        parse(titlebarObj, QStringLiteral("font-size"), status, tb.font.fontSize, baseTb.font.fontSize);
        parse(titlebarObj, QStringLiteral("text-align"), status, tb.font.textAlign, baseTb.font.textAlign);
        parse(titlebarObj, QStringLiteral("text-color"), status, tb.font.textColor, baseTb.font.textColor);

        const QJsonObject buttons = titlebarObj.value(QLatin1String("button-group")).toObject();
        const auto parseButton = [&](const QString &name, ThemeConfig::TitlebarBtn &btn, const ThemeConfig::TitlebarBtn &baseBtn) {
            const QJsonObject btnObj = buttons.value(name).toObject();
            if (btnObj.isEmpty()) {
                return;
            }
            parse(btnObj, QStringLiteral("pos"), status, btn.pos, baseBtn.pos);
            parse(btnObj, QStringLiteral("width"), status, btn.width, baseBtn.width);
            parse(btnObj, QStringLiteral("height"), status, btn.height, baseBtn.height);
            const QJsonObject iconObj = btnObj.value(QLatin1String("icon")).toObject();
            btn.btnIcon = iconObj.isEmpty() ? baseBtn.btnIcon : takeIcon(iconObj);
        };
        parseButton(QStringLiteral("menu"), tb.menuBtn, baseTb.menuBtn);
        parseButton(QStringLiteral("minimize"), tb.minimizeBtn, baseTb.minimizeBtn);
        parseButton(QStringLiteral("maximize"), tb.maximizeBtn, baseTb.maximizeBtn);
        parseButton(QStringLiteral("unmaximize"), tb.unmaximizeBtn, baseTb.unmaximizeBtn);
        parseButton(QStringLiteral("close"), tb.closeBtn, baseTb.closeBtn);
    }

    parse(windowDecObj, QStringLiteral("rounded-corner-radius"), status, config->radius, base->radius);
    parse(windowDecObj, QStringLiteral("blur"), status, config->blur, base->blur);
    parse(windowDecObj, QStringLiteral("opcaity"), status, config->opacity, base->opacity);
    parse(windowDecObj, QStringLiteral("mouseInputAreaMargins"), status, config->mouseInputAreaMargins,
          base == &defaults ? QMarginsF(10, 10, 10, 10) : base->mouseInputAreaMargins);

    const QJsonObject shadowObj = windowDecObj.value(QLatin1String("shadow")).toObject();
    parse(shadowObj, QStringLiteral("shadowRadius"), status, config->shadowConfig.shadowRadius, base->shadowConfig.shadowRadius);
    parse(shadowObj, QStringLiteral("shadowColor"), status, config->shadowConfig.shadowColor,
          base == &defaults ? QColor(0, 0, 0, 255 * 0.6) : base->shadowConfig.shadowColor);
    parse(shadowObj, QStringLiteral("shadowOffset"), status, config->shadowConfig.shadowOffset,
          base == &defaults ? QPointF(0.0, 16.0) : base->shadowConfig.shadowOffset);

    const QJsonObject borderObj = windowDecObj.value(QLatin1String("border")).toObject();
    parse(borderObj, QStringLiteral("width"), status, config->borderConfig.borderWidth, base->borderConfig.borderWidth);
    parse(borderObj, QStringLiteral("color"), status, config->borderConfig.borderColor,
          base == &defaults ? QColor(0, 0, 0, 255 * 0.15) : base->borderConfig.borderColor);
}

static void writeConfig(UIWindowType windowType, const QJsonObject &windowDecoration,
                        ChameleonTheme::ConfigGroupMap *configs, const ChameleonTheme::ConfigGroupMap *base)
{
    const ChameleonTheme::ThemeConfig *baseActive = nullptr;
    const ChameleonTheme::ThemeConfig *baseInactive = nullptr;
    const ChameleonTheme::ThemeConfig *baseUnmanaged = nullptr;
    if (base) {
        const auto managed = base->managedConfigGroupMap.constFind(windowType);
        if (managed != base->managedConfigGroupMap.constEnd()) {
            baseActive = &managed->normal;
            baseInactive = &managed->inactive;
        }
        const auto unmanaged = base->unmanagedConfigMap.constFind(windowType);
        if (unmanaged != base->unmanagedConfigMap.constEnd()) {
            baseUnmanaged = &*unmanaged;
        }
    }

    ChameleonTheme::ConfigGroup managedConfigGroup;
    ChameleonTheme::ThemeConfig unmanagedConfig;

    parseWindowDecoration(windowDecoration, QStringLiteral("active"), &managedConfigGroup.normal, baseActive);
    parseWindowDecoration(windowDecoration, QStringLiteral("inactive"), &managedConfigGroup.inactive,
                          baseInactive ? baseInactive : &managedConfigGroup.normal);
    parseWindowDecoration(windowDecoration, QStringLiteral("unmanaged"), &unmanagedConfig,
                          baseUnmanaged ? baseUnmanaged : &managedConfigGroup.normal);

    configs->managedConfigGroupMap.insert(windowType, managedConfigGroup);
    configs->unmanagedConfigMap.insert(windowType, unmanagedConfig);
}

static bool loadThemeFile(ChameleonTheme::ConfigGroupMap *configs, const ChameleonTheme::ConfigGroupMap *base,
                          ChameleonTheme::ThemeType themeType, const QString &themeName, const QList<QDir> &themeDirList)
{
    QString themeDir;
    for (const QDir &dir : themeDirList) {
        if (dir.exists(themeName)) {
            themeDir = dir.filePath(themeName);
            break;
        }
    }
    if (themeDir.isEmpty()) {
        return false;
    }

    const QString themeJsonPath = QDir(themeDir).filePath(ChameleonTheme::typeString(themeType) + QLatin1String("/decoration.json"));
    QFile f(themeJsonPath);
    if (!f.open(QIODevice::ReadOnly)) {
        qCWarning(CHAMELEON) << "Could not open file:" << themeJsonPath;
        return false;
    }

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(f.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(CHAMELEON) << "Failed to parse" << themeJsonPath << error.errorString();
        return false;
    }

    const QJsonObject rootObj = doc.object();
    for (auto it = rootObj.begin(); it != rootObj.end(); ++it) {
        const int id = it.key().toInt();
        if (id < static_cast<int>(UIWindowType::Normal) || id > static_cast<int>(UIWindowType::Tooltip)) {
            continue;
        }
        writeConfig(static_cast<UIWindowType>(id), it.value().toObject(), configs, base);
    }

    return true;
}

static bool splitThemeName(const QString &fullName, ChameleonTheme::ThemeType &type, QString &name)
{
    const int split = fullName.indexOf(QLatin1Char('/'));
    if (split > 0 && split < fullName.size() - 1) {
        type = ChameleonTheme::typeFromString(fullName.left(split));
        name = fullName.mid(split + 1);
        return true;
    }
    return false;
}

ChameleonTheme::ConfigGroupMapPtr ChameleonTheme::loadTheme(ThemeType themeType, const QString &themeName, const QList<QDir> &themeDirList)
{
    const auto base = getBaseConfig(themeType, themeDirList);
    if (themeName == s_baseTheme) {
        return base;
    }

    auto newConfigMap = std::make_unique<ConfigGroupMap>(*base);
    if (loadThemeFile(newConfigMap.get(), base.data(), themeType, themeName, themeDirList)) {
        return ConfigGroupMapPtr(newConfigMap.release());
    }
    return ConfigGroupMapPtr();
}

ChameleonTheme::ConfigGroupMapPtr ChameleonTheme::getBaseConfig(ThemeType type, const QList<QDir> &themeDirList)
{
    static ConfigGroupMapPtr baseConfigs[ThemeTypeCount];
    if (!baseConfigs[type]) {
        auto *base = new ConfigGroupMap;
        // Load the built-in base theme first, then let installed copies of the
        // base theme override individual values.
        loadThemeFile(base, nullptr, type, s_baseTheme, {QDir(s_baseThemeDir)});
        loadThemeFile(base, base, type, s_baseTheme, themeDirList);
        baseConfigs[type] = ConfigGroupMapPtr(base);
    }
    return baseConfigs[type];
}

QString ChameleonTheme::typeString(ThemeType type)
{
    return type == Dark ? QStringLiteral("dark") : QStringLiteral("light");
}

ChameleonTheme::ThemeType ChameleonTheme::typeFromString(const QString &type)
{
    return type == QLatin1String("dark") ? Dark : Light;
}

QString ChameleonTheme::theme() const
{
    return typeString(m_type) + QLatin1Char('/') + m_theme;
}

ChameleonTheme::ThemeType ChameleonTheme::themeType() const
{
    return m_type;
}

bool ChameleonTheme::setTheme(const QString &themeFullName)
{
    ThemeType type;
    QString name;
    if (!splitThemeName(themeFullName, type, name)) {
        return false;
    }
    return setTheme(type, name);
}

bool ChameleonTheme::setTheme(ThemeType type, const QString &theme)
{
    if (m_configGroupMap && m_type == type && m_theme == theme) {
        return true;
    }

    const ConfigGroupMapPtr configs = loadTheme(type, theme, m_themeDirList);
    if (!configs || !configs->managedConfigGroupMap.contains(UIWindowType::Normal)) {
        qCWarning(CHAMELEON) << "Could not load chameleon theme" << typeString(type) << theme;
        return false;
    }

    m_type = type;
    m_theme = theme;
    m_configGroupMap = configs;
    m_windowThemes.clear();
    return true;
}

static const ChameleonTheme::ConfigGroup *findConfigGroup(const ChameleonTheme::ConfigGroupMapPtr &configs, UIWindowType windowType)
{
    if (!configs) {
        return nullptr;
    }
    const auto &map = configs->managedConfigGroupMap;
    auto it = map.constFind(windowType);
    if (it == map.constEnd()) {
        it = map.constFind(UIWindowType::Normal);
    }
    return it == map.constEnd() ? nullptr : &*it;
}

const ChameleonTheme::ConfigGroup *ChameleonTheme::themeConfig(UIWindowType windowType) const
{
    return findConfigGroup(m_configGroupMap, windowType);
}

const ChameleonTheme::ConfigGroup *ChameleonTheme::themeConfig(const QString &themeName, UIWindowType windowType)
{
    const QString fullName = themeName.contains(QLatin1Char('/')) ? themeName : themeName + QLatin1Char('/') + m_theme;
    ThemeType type;
    QString name;
    if (!splitThemeName(fullName, type, name) || (type == m_type && name == m_theme)) {
        return themeConfig(windowType);
    }

    auto it = m_windowThemes.find(fullName);
    if (it == m_windowThemes.end()) {
        it = m_windowThemes.insert(fullName, loadTheme(type, name, m_themeDirList));
    }
    if (const ConfigGroup *group = findConfigGroup(*it, windowType)) {
        return group;
    }
    return themeConfig(windowType);
}

ChameleonTheme::ChameleonTheme()
{
    const QStringList dataPaths = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                            QStringLiteral("deepin/themes"),
                                                            QStandardPaths::LocateDirectory);
    for (const QString &path : dataPaths) {
        m_themeDirList.prepend(QDir(path));
    }

    setTheme(Light, s_baseTheme);
}

ChameleonTheme::~ChameleonTheme() = default;
