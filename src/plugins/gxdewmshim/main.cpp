/*
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "gxdewmshim.h"

using namespace KWin;

class KWIN_EXPORT GxdeWmShimFactory : public PluginFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginFactory_iid FILE "metadata.json")
    Q_INTERFACES(KWin::PluginFactory)

public:
    explicit GxdeWmShimFactory() = default;

    std::unique_ptr<Plugin> create() const override;
};

std::unique_ptr<Plugin> GxdeWmShimFactory::create() const
{
    return std::make_unique<GxdeWmShim>();
}

#include "main.moc"
