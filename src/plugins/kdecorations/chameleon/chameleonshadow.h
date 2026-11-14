/*
    SPDX-FileCopyrightText: 2017 - 2019 Deepin Technology Co., Ltd.
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#pragma once

#include "chameleontheme.h"

#include <KDecoration3/DecorationShadow>

#include <memory>

class ChameleonShadow
{
public:
    static ChameleonShadow *instance();

    /**
     * Shadow tile for @p config. When @p drawBorder is set the window border is
     * painted into the shadow, which is how chameleon draws borders while
     * compositing (the decoration itself then has no side borders).
     */
    std::shared_ptr<KDecoration3::DecorationShadow> getShadow(const ChameleonTheme::ThemeConfig &config, bool drawBorder);

    void clearCache();

private:
    QMap<QString, std::shared_ptr<KDecoration3::DecorationShadow>> m_shadowCache;
};
