# gxde-kwin_x11

This repository is the GXDE X11-only fork of KWin. The CMake project, main
executable, and shared library are named `gxde-kwin_x11`. Runtime data,
plugins, configuration (`gxde-kwin_x11rc`), translations, documentation,
KCMs, helpers, and CMake packages use separate GXDE namespaces so the fork can
be installed alongside upstream KWin.

The build intentionally has no `gxde-kwin_wayland` (or `kwin_wayland`)
compositor target. Wayland protocol code that the X11 implementation needs
internally is still compiled; it does not produce a Wayland compositor
executable.

The session service is installed as `plasma-gxde-kwin_x11.service`. It keeps
the standard `org.kde.KWin` D-Bus API for desktop-shell compatibility and
conflicts with the upstream KWin session services, because two window managers
cannot control the same display at the same time.

## About KWin/X11

KWin/X11 is an X11 window manager and a compositing manager. Its primary usage is in conjunction with a Desktop Shell (e.g. KDE Plasma Desktop). KWin/X11 is designed to go out of the way; users should not notice that they use a window manager at all. Nevertheless KWin/X11 provides a steep learning curve for advanced features, which are available, if they do not conflict with the primary mission. KWin does not have a dedicated targeted user group, but follows the targeted user group of the Desktop Shell using KWin/X11 as it's window manager.

## KWin is not...

 * a standalone window manager (c.f. openbox, i3) and does not provide any functionality belonging to a Desktop Shell.
 * a replacement for window managers designed for use with a specific Desktop Shell (e.g. GNOME Shell)
 * a minimalistic window manager
 * designed for use without compositing or for X11 network transparency, though both are possible.

# Contributing to KWin

Please refer to the [contributing document](CONTRIBUTING.md) for everything you need to know to get started contributing to KWin.

# Contacting KWin development team

 * mailing list: [kwin@kde.org](https://mail.kde.org/mailman/listinfo/kwin)
 * IRC: #kde-kwin on irc.libera.chat
 * Matrix: [#kwin:kde.org](https://go.kde.org/matrix/#/#kwin:kde.org)

# Support
## Application Developer
If you are an application developer having questions regarding windowing systems (either X11 or Wayland) please do not hesitate to contact us. Preferable through our mailing list. Ideally subscribe to the mailing list, so that your mail doesn't get stuck in the moderation queue.

## End user
Please contact the support channels of your Linux distribution for user support. The KWin development team does not provide end user support.

# Reporting bugs

Please use [KDE's bugtracker](https://bugs.kde.org) and report for [product KWin](https://bugs.kde.org/enter_bug.cgi?product=kwin).
