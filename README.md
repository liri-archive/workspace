Hawaii Workspace
================

[![IRC Network](https://img.shields.io/badge/irc-freenode-blue.svg "IRC Freenode")](https://webchat.freenode.net/?channels=hawaii-desktop)
[![GitHub release](https://img.shields.io/github/release/hawaii-desktop/hawaii-workspace.svg)](https://github.com/hawaii-desktop/hawaii-workspace)
[![GitHub issues](https://img.shields.io/github/issues/hawaii-desktop/hawaii-workspace.svg)](https://github.com/hawaii-desktop/hawaii-workspace/issues)

Base applications for the Hawaii desktop environment.

Includes:

* hawaii-polkit-agent, the PolicyKit agent
* hawaii-screenshot, screenshot application
* hawaii-screencap, screencast application
* Qt platform theme plugin
* Settings schemas
* XDG menu definition

## License

Licensed under the terms of the GNU General Public License version 2.0 or,
at your option, any later version.

## Dependencies

Make sure you have Qt >= 5.6.0 with the following modules:

* [qtbase](http://code.qt.io/cgit/qt/qtbase.git)
* [qtdeclarative](http://code.qt.io/cgit/qt/qtdeclarative.git)
* [qtquickcontrols](http://code.qt.io/cgit/qt/qtquickcontrols.git)
* [qtquickcontrols2](http://code.qt.io/cgit/qt/qtquickcontrols2.git)

The following modules from KDE:

* [polkit-qt5](http://quickgit.kde.org/?p=polkit-qt-1.git)

The following modules from GStreamer:

* [qt-gstreamer](https://cgit.freedesktop.org/gstreamer/qt-gstreamer)

And the following requirements from Hawaii:

* [greenisland](https://github.com/greenisland/greenisland.git)
* [fluid](https://github.com/hawaii-desktop/fluid.git)
* [libhawaii](https://github.com/hawaii-desktop/libhawaii.git)

## Build and installation

Building this software is a piece of cake.

Assuming you are in the source directory, just create a build directory
and run cmake:

```sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt/hawaii ..
```

To do a debug build the last command can be:

```sh
cmake -DCMAKE_INSTALL_PREFIX=/opt/hawaii -DCMAKE_BUILD_TYPE=Debug ..
```

To do a release build instead it can be:

```sh
cmake -DCMAKE_INSTALL_PREFIX=/opt/hawaii -DCMAKE_BUILD_TYPE=Release ..
```

If not passed, the `CMAKE_INSTALL_PREFIX` parameter defaults to /usr/local.
You have to specify a path that fits your needs, /opt/hawaii is just an example.

The `CMAKE_BUILD_TYPE` parameter allows the following values:

* **Debug:** debug build
* **Release:** release build
* **RelWithDebInfo:** release build with debugging information

### System-wide installation

Those who want to perform a system-wide installation, such as package
maintainers, should pass different arguments to cmake:

```sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DKDE_INSTALL_USE_QT_SYS_PATHS=ON ..
```

Feel free to choose whatever `CMAKE_BUILD_TYPE` value you desire.
