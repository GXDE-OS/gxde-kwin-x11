#! /usr/bin/env bash
$EXTRACTRC *.ui >> rc.cpp || exit 11
$XGETTEXT *.cpp -o $podir/gxde-kcmkwinscreenedges_x11.pot
rm -f rc.cpp
