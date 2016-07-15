Workspace
=========

[![ZenHub.io](https://img.shields.io/badge/supercharged%20by-zenhub.io-blue.svg)](https://zenhub.io)

[![License](https://img.shields.io/badge/license-GPLv2.0%2B-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)
[![GitHub release](https://img.shields.io/github/release/qmlos/workspace.svg)](https://github.com/qmlos/workspace)
[![Build Status](https://travis-ci.org/qmlos/workspace.svg?branch=develop)](https://travis-ci.org/qmlos/workspace)
[![GitHub issues](https://img.shields.io/github/issues/qmlos/workspace.svg)](https://github.com/qmlos/workspace/issues)
[![Maintained](https://img.shields.io/maintenance/yes/2016.svg)](https://github.com/qmlos/workspace/commits/develop)

Base applications and settings for the qmlOS workspace.

Includes:

* hawaii-polkit-agent, the PolicyKit agent
* hawaii-screenshot, screenshot application
* hawaii-screencap, screencast application
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

 * [polkit-qt5](http://quickgit.kde.org/?p=polkit-qt-1.git)
 * [qt-gstreamer](https://cgit.freedesktop.org/gstreamer/qt-gstreamer)
 * [greenisland](https://github.com/greenisland/greenisland.git)
 * [libhawaii](https://github.com/hawaii-desktop/libhawaii.git)

## Installation

From the root of the repository, run:

```sh
mkdir build; cd build
cmake .. -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
make
make install # use sudo if necessary
```

On the `cmake` line, you can specify additional configuration parameters:

 * `-DCMAKE_INSTALL_PREFIX=/path/to/install` (for example, `/opt/qmlos` or `/usr`)
 * `-DCMAKE_BUILD_TYPE=<build_type>`, where `<build_type>` is one of:
   * **Debug:** debug build
   * **Release:** release build
   * **RelWithDebInfo:** release build with debugging information

## Licensing

Licensed under the terms of the GNU General Public License version 2.0 or,
at your option, any later version.
