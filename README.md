Workspace
=========

[![ZenHub.io](https://img.shields.io/badge/supercharged%20by-zenhub.io-blue.svg)](https://zenhub.io)

[![License](https://img.shields.io/badge/license-GPLv3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)
[![GitHub release](https://img.shields.io/github/release/lirios/workspace.svg)](https://github.com/lirios/workspace)
[![Build Status](https://travis-ci.org/lirios/workspace.svg?branch=develop)](https://travis-ci.org/lirios/workspace)
[![GitHub issues](https://img.shields.io/github/issues/lirios/workspace.svg)](https://github.com/lirios/workspace/issues)
[![Maintained](https://img.shields.io/maintenance/yes/2018.svg)](https://github.com/lirios/workspace/commits/develop)

Base applications and settings for the Liri OS workspace.

Includes:

* liri-screenshot, screenshot application
* liri-screencast, screencast application
* liri-powermanager, power management
* Settings schemas
* XDG menu definition

## Dependencies

Make sure you have Qt >= 5.8.0 with the following modules:

 * [qtbase](http://code.qt.io/cgit/qt/qtbase.git)
 * [qtdeclarative](http://code.qt.io/cgit/qt/qtdeclarative.git)
 * [qtquickcontrols2](http://code.qt.io/cgit/qt/qtquickcontrols2.git)

And the following modules:

 * [qbs-shared](https://github.com/lirios/qbs-shared.git)
 * [solid](http://quickgit.kde.org/?p=solid.git)
 * [qtgsettings](https://github.com/lirios/qtgsettings.git)
 * [libliri](https://github.com/lirios/libliri.git)
 * [liri-wayland](https://github.com/lirios/wayland.git)

Themes and wallpapers to honor default settings:

 * [paper-icon-themes](https://github.com/snwh/paper-icon-theme)
 * [liri-wallpapers](https://github.com/lirios/wallpapers)

If you do not install those dependencies, please configure the desktop with an
alternative icon theme and wallpaper.

Optional services at runtime:

 * `upower` is used by `liri-powermanager` to determine whether the
   system is on battery power or AC power, whether lid is closed or not.
   If `upower` is not available, the system is assumed to be a desktop
   computer on AC power.

 * `systemd` is used by `liri-powermanager` to determine whether
   the system is a virtual machine and the chassis type.
   If `systemd` is not available, the system is assumed to be
   a bare-metal desktop computer.

## Installation

Qbs is a new build system that is much easier to use compared to qmake or CMake.

If you want to learn more, please read the [Qbs manual](http://doc.qt.io/qbs/index.html),
especially the [setup guide](http://doc.qt.io/qbs/configuring.html) and how to install artifacts
from the [installation guide](http://doc.qt.io/qbs/installing-files.html).

If you haven't already, start by setting up a `qt5` profile for `qbs`:

```sh
qbs setup-toolchains --type gcc /usr/bin/g++ gcc
qbs setup-qt $(which qmake) qt5 # make sure that qmake is in PATH
qbs config profiles.qt5.baseProfile gcc
```

Then, from the root of the repository, run:

```sh
qbs -d build -j $(nproc) profile:qt5 # use sudo if necessary
```

To the `qbs` call above you can append additional configuration parameters:

 * `modules.lirideployment.prefix:/path/to/prefix` where most files are installed (default: `/usr/local`)
 * `modules.lirideployment.dataDir:path/to/lib` where data files are installed (default: `/usr/local/share`)
 * `modules.lirideployment.libDir:path/to/lib` where libraries are installed (default: `/usr/local/lib`)
 * `modules.lirideployment.qmlDir:path/to/qml` where QML plugins are installed (default: `/usr/local/lib/qml`)
 * `modules.lirideployment.pluginsDir:path/to/plugins` where Qt plugins are installed (default: `/usr/local/lib/plugins`)
 * `modules.lirideployment.qbsModulesDir:path/to/qbs` where Qbs modules are installed (default: `/usr/local/share/qbs/modules`)

See [lirideployment.qbs](https://github.com/lirios/qbs-shared/blob/develop/modules/lirideployment/lirideployment.qbs)
for more deployment-related parameters.

## Licensing

Licensed under the terms of the GNU General Public License version 3.0 or,
at your option, any later version.
