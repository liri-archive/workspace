Workspace
=========

[![ZenHub.io](https://img.shields.io/badge/supercharged%20by-zenhub.io-blue.svg)](https://zenhub.io)

[![License](https://img.shields.io/badge/license-GPLv3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)
[![GitHub release](https://img.shields.io/github/release/lirios/workspace.svg)](https://github.com/lirios/workspace)
[![Build Status](https://travis-ci.org/lirios/workspace.svg?branch=develop)](https://travis-ci.org/lirios/workspace)
[![GitHub issues](https://img.shields.io/github/issues/lirios/workspace.svg)](https://github.com/lirios/workspace/issues)
[![Maintained](https://img.shields.io/maintenance/yes/2016.svg)](https://github.com/lirios/workspace/commits/develop)

Base applications and settings for the Liri OS workspace.

Includes:

* liri-screenshot, screenshot application
* liri-screencast, screencast application
* Qt platform theme plugin
* Settings schemas
* XDG menu definition

## Dependencies

Make sure you have Qt >= 5.7.0 with the following modules:

 * [qtbase](http://code.qt.io/cgit/qt/qtbase.git)
 * [qtdeclarative](http://code.qt.io/cgit/qt/qtdeclarative.git)
 * [qtquickcontrols](http://code.qt.io/cgit/qt/qtquickcontrols.git)
 * [qtquickcontrols2](http://code.qt.io/cgit/qt/qtquickcontrols2.git)

And the following modules:

 * [qt-gstreamer](https://cgit.freedesktop.org/gstreamer/qt-gstreamer)
 * [greenisland](https://github.com/greenisland/greenisland.git)
 * [vibe](https://github.com/lirios/vibe.git)

Themes and wallpapers to honor default settings:

 * [paper-icon-themes](https://github.com/snwh/paper-icon-theme)
 * [hawaii-wallpapers](https://github.com/hawaii-desktop/hawaii-wallpapers)

If you do not install those dependencies, please configure the desktop with an
alternative icon theme and wallpaper.

## Installation

From the root of the repository, run:

```sh
mkdir build; cd build
cmake .. -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
make
make install # use sudo if necessary
sudo glib-compile-schemas /usr/share/glib-2.0/schemas
```

On the `cmake` line, you can specify additional configuration parameters:

 * `-DCMAKE_INSTALL_PREFIX=/path/to/install` (for example, `/opt/liri` or `/usr`)
 * `-DCMAKE_BUILD_TYPE=<build_type>`, where `<build_type>` is one of:
   * **Debug:** debug build
   * **Release:** release build
   * **RelWithDebInfo:** release build with debugging information

## Licensing

Licensed under the terms of the GNU General Public License version 3.0 or,
at your option, any later version.
