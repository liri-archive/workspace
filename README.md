Hawaii Workspace
================

[![IRC Network](https://img.shields.io/badge/irc-freenode-blue.svg "IRC Freenode")](https://webchat.freenode.net/?channels=hawaii-desktop)
[![GitHub release](https://img.shields.io/github/release/hawaii-desktop/hawaii-baseapps.svg)](https://github.com/hawaii-desktop/hawaii-baseapps)
[![GitHub issues](https://img.shields.io/github/issues/hawaii-desktop/hawaii-baseapps.svg)](https://github.com/hawaii-desktop/hawaii-baseapps/issues)

Base applications for the Hawaii desktop environment.

Includes:

* hawaii-polkit-agent, the PolicyKit agent
* hawaii-screenshooter, screenshot application

## Dependencies

Make sure you have Qt >= 5.6.0 with the following modules:

* [qtbase](http://code.qt.io/cgit/qt/qtbase.git)
* [qtdeclarative](http://code.qt.io/cgit/qt/qtdeclarative.git)
* [qtquickcontrols](http://code.qt.io/cgit/qt/qtquickcontrols.git)
* [qtquickcontrols2](http://code.qt.io/cgit/qt/qtquickcontrols2.git)

The following modules from KDE:

* [polkit-qt5](http://quickgit.kde.org/?p=polkit-qt-1.git)

And the follow build requirements:

* [greenisland](https://github.com/greenisland/greenisland.git)

## Build

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

Package maintainers would pass `-DCMAKE_INSTALL_PREFIX=/usr`.

The `CMAKE_BUILD_TYPE` parameter allows the following values:

* **Debug:** debug build
* **Release:** release build
* **RelWithDebInfo:** release build with debugging information

## Installation

It's really easy, it's just a matter of typing:

```sh
make install
```

from the build directory.
