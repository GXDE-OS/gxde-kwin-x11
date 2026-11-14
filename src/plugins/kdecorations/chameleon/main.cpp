/*
    SPDX-FileCopyrightText: 2017 - 2019 Deepin Technology Co., Ltd.
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "chameleon.h"

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(ChameleonDecoFactory, "chameleon.json", registerPlugin<Chameleon>();)

#include "main.moc"
