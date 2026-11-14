/*
    SPDX-FileCopyrightText: 2017 - 2019 Deepin Technology Co., Ltd.
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
// Ported from deepin-kwin's deepin-chameleon decoration (KDecoration2) to
// KDecoration3 for gxde-kwin_x11.

#pragma once

#include <QColor>
#include <QDir>
#include <QIcon>
#include <QLoggingCategory>
#include <QMap>
#include <QMarginsF>
#include <QPointF>
#include <QSharedData>

Q_DECLARE_LOGGING_CATEGORY(CHAMELEON)

enum class UIWindowType {
    Normal = 1001,
    Dialog,
    Dock,
    PopupMenu,
    DropdownMenu,
    Tooltip,
};

class ChameleonTheme
{
public:
    enum ThemeType {
        Light,
        Dark,
        ThemeTypeCount
    };

    struct ThemeConfig
    {
        struct TitlebarBtn
        {
            QPointF pos;
            qint32 width = 50;
            qint32 height = 50;
            QIcon btnIcon;
        };
        struct TitlebarFont
        {
            QString fontFamily;
            int fontSize = 14;
            QString textAlign;
            QColor textColor;
        };
        struct TitlebarConfig
        {
            qreal height = 50;
            Qt::Edge area = Qt::TopEdge;
            QColor backgroundColor;

            TitlebarFont font;

            TitlebarBtn menuBtn;
            TitlebarBtn minimizeBtn;
            TitlebarBtn maximizeBtn;
            TitlebarBtn unmaximizeBtn;
            TitlebarBtn closeBtn;
        };
        struct BorderConfig
        {
            qreal borderWidth = 1;
            QColor borderColor;
        };
        struct ShadowConfig
        {
            qreal shadowRadius = 60;
            QPointF shadowOffset;
            QColor shadowColor;
        };

        QString windowDesc;
        TitlebarConfig titlebarConfig;
        BorderConfig borderConfig;
        QPointF radius;
        ShadowConfig shadowConfig;
        qreal blur = 20;
        qreal opacity = 20;
        QMarginsF mouseInputAreaMargins;
    };

    struct ConfigGroup
    {
        ThemeConfig normal;
        ThemeConfig inactive;
    };

    struct ConfigGroupMap : public QSharedData
    {
        QMap<UIWindowType, ConfigGroup> managedConfigGroupMap;
        QMap<UIWindowType, ThemeConfig> unmanagedConfigMap;
    };

    typedef QSharedDataPointer<ConfigGroupMap> ConfigGroupMapPtr;

    static QPair<qreal, qreal> takePair(const QVariant &value, const QPair<qreal, qreal> defaultValue);
    static QMarginsF takeMargins(const QVariant &value, const QMarginsF &defaultValue);
    static QPointF takePos(const QVariant &value, const QPointF defaultValue);

    static ChameleonTheme *instance();
    static ConfigGroupMapPtr loadTheme(ThemeType themeType, const QString &themeName, const QList<QDir> &themeDirList);
    static ConfigGroupMapPtr getBaseConfig(ThemeType type, const QList<QDir> &themeDirList);
    static QString typeString(ThemeType type);
    static ThemeType typeFromString(const QString &type);

    /**
     * Full theme name as written by the GXDE window manager D-Bus service to
     * [deepin-chameleon] theme, e.g. "light/deepin" or "dark/deepin".
     */
    QString theme() const;
    bool setTheme(const QString &themeFullName);
    bool setTheme(ThemeType type, const QString &theme);
    ThemeType themeType() const;

    const ConfigGroup *themeConfig(UIWindowType windowType) const;
    /**
     * Configuration for a window that asked for its own theme. @p themeName is
     * either a full name such as "dark/deepin" or only the type ("dark"), which
     * then refers to the current theme of that type.
     */
    const ConfigGroup *themeConfig(const QString &themeName, UIWindowType windowType);

protected:
    ChameleonTheme();
    ~ChameleonTheme();

private:
    QList<QDir> m_themeDirList;
    ThemeType m_type = Light;
    QString m_theme;
    ConfigGroupMapPtr m_configGroupMap;
    QMap<QString, ConfigGroupMapPtr> m_windowThemes;
};
