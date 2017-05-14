Workspace
=========

[![ZenHub.io](https://img.shields.io/badge/supercharged%20by-zenhub.io-blue.svg)](https://zenhub.io)

[![License](https://img.shields.io/badge/license-GPLv3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)
[![GitHub release](https://img.shields.io/github/release/lirios/workspace.svg)](https://github.com/lirios/workspace)
[![Build Status](https://travis-ci.org/lirios/workspace.svg?branch=develop)](https://travis-ci.org/lirios/workspace)
[![GitHub issues](https://img.shields.io/github/issues/lirios/workspace.svg)](https://github.com/lirios/workspace/issues)
[![Maintained](https://img.shields.io/maintenance/yes/2017.svg)](https://github.com/lirios/workspace/commits/develop)

Base applications and settings for the Liri OS workspace.

Includes:

* liri-screenshot, screenshot application
* liri-screencast, screencast application
* Settings schemas
* XDG menu definition

## Dependencies

Make sure you have Qt >= 5.8.0 with the following modules:

 * [qtbase](http://code.qt.io/cgit/qt/qtbase.git)
 * [qtdeclarative](http://code.qt.io/cgit/qt/qtdeclarative.git)
 * [qtquickcontrols2](http://code.qt.io/cgit/qt/qtquickcontrols2.git)

And the following modules:

 * [qt-gstreamer](https://cgit.freedesktop.org/gstreamer/qt-gstreamer)
 * [liri-wayland](https://github.com/lirios/wayland.git)

Themes and wallpapers to honor default settings:

 * [paper-icon-themes](https://github.com/snwh/paper-icon-theme)
 * [liri-wallpapers](https://github.com/lirios/wallpapers)

If you do not install those dependencies, please configure the desktop with an
alternative icon theme and wallpaper.

## Installation

Qbs is a new build system that is much easier to use compared to qmake or CMake.

If you want to learn more, please read the [Qbs manual](http://doc.qt.io/qbs/index.html),
especially the [setup guide](http://doc.qt.io/qbs/configuring.html) and how to install artifacts
from the [installation guide](http://doc.qt.io/qbs/installing-files.html).

From the root of the repository, run:

```sh
qbs setup-toolchains --type gcc /usr/bin/g++ gcc
qbs setup-qt /usr/bin/qmake-qt5 qt5
qbs config profiles.qt5.baseProfile gcc
qbs -d build -j $(nproc) profile:qt5 # use sudo if necessary
```

On the last `qbs` line, you can specify additional configuration parameters at the end:

 * `qbs.installRoot:/path/to/install` (for example `/opt/liri` or `/usr`)

The following are only needed if `qbs.installRoot` is a system-wide path such as `/usr`
and the default value doesn't suit your needs. All are relative to `qbs.installRoot`:

 * `lirideployment:libDir=path/to/lib` where libraries are installed (default: `lib`)
 * `lirideployment:qmlDir=path/to/qml` where QML plugins are installed (default: `lib/qml`)
 * `lirideployment:pluginsDir=path/to/plugins` where Qt plugins are installed (default: `lib/plugins`)
 * `lirideployment:qbsModulesDir=path/to/qbs` where Qbs modules are installed (default: `share/qbs/modules`)

See `qbs-shared/modules/lirideployment/lirideployment.qbs` for more deployment-related parameters.

You can also specify the following options:

 * `qbsbuildconfig:developmentBuild=true` to enable features for development such as the ability
   to load arbitrary QML files from the shell
 * `qbsbuildconfig:systemdUserUnitDir=path/to/units` to install systemd user units in a different
   path, the path is relative to `qbs.installRoot` and defaults to `lib/systemd/user`

If you specify `qbs.installRoot` you might need to prefix the entire line with `sudo`,
depending on whether you have permissions to write there or not.

## Licensing

Licensed under the terms of the GNU General Public License version 3.0 or,
at your option, any later version.
