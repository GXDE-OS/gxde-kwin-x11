<a id="readme-top"></a>

<div align="center">
  <a href="https://github.com/GXDE-OS/gxde-kwin-x11">
    <img src="logo.png" alt="GXDE KWin X11 徽标" width="96" height="96">
  </a>

  <h1 align="center">GXDE KWin X11</h1>

  <p align="center">
    面向 GXDE 桌面环境的 X11 专用 KDE KWin 分支。
    <br />
    <a href="https://github.com/GXDE-OS/gxde-kwin-x11/issues">报告问题</a>
    &middot;
    <a href="https://github.com/GXDE-OS/gxde-kwin-x11/issues">提出功能建议</a>
    &middot;
    <a href="README.md">English</a>
  </p>
</div>

[![GitHub stars](https://img.shields.io/github/stars/GXDE-OS/gxde-kwin-x11?style=for-the-badge)](https://github.com/GXDE-OS/gxde-kwin-x11/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/GXDE-OS/gxde-kwin-x11?style=for-the-badge)](https://github.com/GXDE-OS/gxde-kwin-x11/network/members)
[![GitHub issues](https://img.shields.io/github/issues/GXDE-OS/gxde-kwin-x11?style=for-the-badge)](https://github.com/GXDE-OS/gxde-kwin-x11/issues)
[![C++](https://img.shields.io/badge/C%2B%2B-23-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16%2B-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org/)

## 目录

- [项目简介](#项目简介)
- [主要差异](#主要差异)
- [技术栈](#技术栈)
- [开始使用](#开始使用)
- [使用方式](#使用方式)
- [参与贡献](#参与贡献)
- [许可证](#许可证)
- [致谢](#致谢)

## 项目简介

GXDE KWin X11 是 [KWin](https://invent.kde.org/plasma/kwin) 的 GXDE 分支。KWin 是 KDE 的窗口管理器和合成器。本项目用于与 GXDE 桌面环境集成，同时保留标准的 `org.kde.KWin` D-Bus API，以兼容桌面 Shell。

本项目专注于 X11，仅提供 X11 桌面会话所需的窗口管理器、合成器、特效、配置模块、插件及相关运行时组件。

## 主要差异

- 项目及主可执行文件名称为 `gxde-kwin_x11`。
- 运行时数据、插件、配置、翻译、文档、KCM、辅助程序和 CMake 包使用 GXDE 专属命名空间。
- 会话服务安装为 `plasma-gxde-kwin_x11.service`。
- 本分支不会构建 `gxde-kwin_wayland` 或 `kwin_wayland` 合成器可执行文件。
- 保留标准 `org.kde.KWin` D-Bus API，以兼容桌面 Shell。

## 技术栈

- [C++23](https://isocpp.org/)
- [CMake](https://cmake.org/)
- [Qt 6](https://www.qt.io/)
- [KDE Frameworks](https://kde.org/products/frameworks/)
- [KDE Plasma Workspace](https://invent.kde.org/plasma/plasma-workspace)
- [X11](https://www.x.org/)

## 开始使用

### 环境要求

当前项目需要：

- 支持 C++23 的编译器
- CMake 3.16 或更高版本
- Qt 6.8.0 或更高版本
- Plasma workspace 依赖 6.3.0 或更高版本
- 发行版提供的 KWin、Qt、KDE Frameworks 和 X11 相关开发包

### 从源代码构建

```bash
git clone https://github.com/GXDE-OS/gxde-kwin-x11.git
cd gxde-kwin-x11
mkdir _build
cd _build
cmake ..
cmake --build . --parallel
```

如需安装到开发环境，请按照发行版的标准 CMake 安装流程操作。有关直接从构建目录运行合成器、调试和测试的说明，请参阅 [`CONTRIBUTING.md`](CONTRIBUTING.md)。

## 使用方式

安装后，X11 合成器可执行文件为：

```bash
gxde-kwin_x11
```

GXDE 会话服务为：

```text
plasma-gxde-kwin_x11.service
```

从构建目录运行时，请遵循 [`CONTRIBUTING.md`](CONTRIBUTING.md) 中的环境设置说明，包括隔离的 `QT_PLUGIN_PATH`。同一个显示器上不能同时运行两个窗口管理器。

## 参与贡献

欢迎参与贡献。开始修改前，请阅读 [`CONTRIBUTING.md`](CONTRIBUTING.md)，其中包含：

- 代码仓库结构和主要源代码目录
- KDE 与 KWin 编码规范
- 提交信息格式建议
- 从源代码构建和运行的方法
- 测试与调试实践

项目相关的问题和功能建议请使用 GitHub Issues。有关上游 KWin 的问题，请访问 [KDE Bugzilla 的 KWin 产品页面](https://bugs.kde.org/enter_bug.cgi?product=kwin) 和 [KWin Matrix 房间](https://matrix.to/#/#kwin:kde.org)。

## 许可证

本仓库包含采用多种自由及开源许可证发布的组件。适用的许可证文本位于 [`LICENSES/`](LICENSES/) 目录中；具体文件的许可信息请以源文件头部声明为准。

## 致谢

- [KWin](https://invent.kde.org/plasma/kwin)：上游 KDE 窗口管理器
- [KDE](https://kde.org/) 及其贡献者
- [GXDE](https://github.com/GXDE-OS) 及其贡献者
- [Best-README-Template](https://github.com/othneildrew/Best-README-Template)：本 README 结构的灵感来源

<p align="right">(<a href="#readme-top">返回顶部</a>)</p>
