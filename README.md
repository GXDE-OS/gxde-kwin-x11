<a id="readme-top"></a>

<div align="center">
  <a href="https://github.com/GXDE-OS/gxde-kwin-x11">
    <img src="logo.png" alt="GXDE KWin X11 logo" width="96" height="96">
  </a>

  <h1 align="center">GXDE KWin X11</h1>

  <p align="center">
    An X11-only fork of KDE KWin for the GXDE desktop environment.
    <br />
    <a href="https://github.com/GXDE-OS/gxde-kwin-x11/issues">Report a bug</a>
    &middot;
    <a href="https://github.com/GXDE-OS/gxde-kwin-x11/issues">Request a feature</a>
    &middot;
    <a href="README.zh.md">简体中文</a>
  </p>
</div>

[![GitHub stars](https://img.shields.io/github/stars/GXDE-OS/gxde-kwin-x11?style=for-the-badge)](https://github.com/GXDE-OS/gxde-kwin-x11/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/GXDE-OS/gxde-kwin-x11?style=for-the-badge)](https://github.com/GXDE-OS/gxde-kwin-x11/network/members)
[![GitHub issues](https://img.shields.io/github/issues/GXDE-OS/gxde-kwin-x11?style=for-the-badge)](https://github.com/GXDE-OS/gxde-kwin-x11/issues)
[![C++](https://img.shields.io/badge/C%2B%2B-23-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16%2B-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org/)

## Table of Contents

- [About the Project](#about-the-project)
- [Key Differences](#key-differences)
- [Built With](#built-with)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## About the Project

GXDE KWin X11 is the GXDE fork of [KWin](https://invent.kde.org/plasma/kwin), KDE's window manager and compositing manager. It is intended to integrate with the GXDE desktop while preserving the standard `org.kde.KWin` D-Bus API for desktop-shell compatibility.

This project focuses exclusively on X11. It provides the window manager, compositor, effects, configuration modules, plugins, and supporting runtime components required by an X11 desktop session.

## Key Differences

- The project and main executable are named `gxde-kwin_x11`.
- Runtime data, plugins, configuration, translations, documentation, KCMs, helpers, and CMake packages use GXDE-specific namespaces.
- The session service is installed as `plasma-gxde-kwin_x11.service`.
- The fork does not build a `gxde-kwin_wayland` or `kwin_wayland` compositor executable.
- The standard `org.kde.KWin` D-Bus API is retained for compatibility with desktop shells.

## Built With

- [C++23](https://isocpp.org/)
- [CMake](https://cmake.org/)
- [Qt 6](https://www.qt.io/)
- [KDE Frameworks](https://kde.org/products/frameworks/)
- [KDE Plasma Workspace](https://invent.kde.org/plasma/plasma-workspace)
- [X11](https://www.x.org/)

## Getting Started

### Prerequisites

The project currently requires:

- A C++23-compatible compiler
- CMake 3.16 or newer
- Qt 6.8.0 or newer
- Plasma workspace dependencies 6.3.0 or newer
- The development packages required by KWin, Qt, KDE Frameworks, and X11 for your distribution

### Build from Source

```bash
git clone https://github.com/GXDE-OS/gxde-kwin-x11.git
cd gxde-kwin-x11
mkdir _build
cd _build
cmake ..
cmake --build . --parallel
```

For a development installation, use the standard CMake install workflow for your distribution. Refer to [`CONTRIBUTING.md`](CONTRIBUTING.md) for running the compositor directly from a build directory, debugging, and testing guidance.

## Usage

After installation, the X11 compositor executable is:

```bash
gxde-kwin_x11
```

The GXDE session service is:

```text
plasma-gxde-kwin_x11.service
```

When running from a build directory, follow the environment setup in [`CONTRIBUTING.md`](CONTRIBUTING.md), including the isolated `QT_PLUGIN_PATH`. Do not run two window managers against the same display at the same time.

## Contributing

Contributions are welcome. Before making changes, please read [`CONTRIBUTING.md`](CONTRIBUTING.md) for:

- Repository layout and useful source directories
- KDE and KWin coding conventions
- Commit message guidance
- Building and running from source
- Test and debugging practices

Please use GitHub issues for project-specific bug reports and feature requests. For upstream KWin topics, consult the [KDE Bugzilla KWin product](https://bugs.kde.org/enter_bug.cgi?product=kwin) and the [KWin Matrix room](https://matrix.to/#/#kwin:kde.org).

## License

This repository contains components under multiple free and open-source licenses. See the files in [`LICENSES/`](LICENSES/) for the applicable license texts and individual source-file headers for licensing details.

## Acknowledgments

- [KWin](https://invent.kde.org/plasma/kwin), the upstream KDE window manager
- [KDE](https://kde.org/) and its contributors
- [GXDE](https://github.com/GXDE-OS) and its contributors
- [Best-README-Template](https://github.com/othneildrew/Best-README-Template), which inspired this README structure

<p align="right">(<a href="#readme-top">back to top</a>)</p>
