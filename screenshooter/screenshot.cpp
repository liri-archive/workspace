/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtGui/QGuiApplication>
#include <QtGui/qpa/qplatformnativeinterface.h>

#include "screenshot.h"
#include "screenshot_p.h"
#include "screenshooter_p.h"

#include <wayland-client.h>

/*
 * ScreenshotPrivate
 */

ScreenshotPrivate::ScreenshotPrivate()
    : QtWayland::greenisland_screenshot()
{
}

void ScreenshotPrivate::screenshot_done(int32_t result)
{
    Q_Q(Screenshot);
    Q_EMIT q->done(static_cast<Screenshot::Result>(result));
}

/*
 * Screenshot
 */

Screenshot::Screenshot(Screenshooter::What what, QScreen *screen, uchar *data,
                       wl_buffer *buffer, QObject *parent)
    : QObject(*new ScreenshotPrivate(), parent)
{
    Screenshooter *shooter = static_cast<Screenshooter *>(parent);
    if (shooter) {
        wl_output *output = static_cast<wl_output *>(QGuiApplication::platformNativeInterface()->nativeResourceForScreen("output", screen));
        ScreenshooterPrivate *ds = ScreenshooterPrivate::get(shooter);
        greenisland_screenshot *object =
                greenisland_screenshooter_shoot(ds->object(), what, output, buffer);
        d_func()->init(object);
    }
}

#include "moc_screenshot.cpp"
