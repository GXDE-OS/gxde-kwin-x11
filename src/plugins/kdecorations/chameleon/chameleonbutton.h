/*
    SPDX-FileCopyrightText: 2017 - 2019 Deepin Technology Co., Ltd.
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#pragma once

#include <KDecoration3/DecorationButton>

class ChameleonButton : public KDecoration3::DecorationButton
{
    Q_OBJECT
public:
    explicit ChameleonButton(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent = nullptr);

    static KDecoration3::DecorationButton *create(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent);

    void paint(QPainter *painter, const QRectF &repaintArea) override;
};
