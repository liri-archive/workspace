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

#include "screenshooter.h"
#include "screenshooter_p.h"

#include <wayland-client.h>

/*
 * ScreenshooterPrivate
 */

ScreenshooterPrivate::ScreenshooterPrivate()
    : QtWayland::greenisland_screenshooter()
{
}

/*
 * Screenshooter
 */

Screenshooter::Screenshooter(QObject *parent)
    : QObject(*new ScreenshooterPrivate(), parent)
{
}

void Screenshooter::shoot(What what, wl_shm *shm, QScreen *screen)
{
    Q_UNUSED(what);
    Q_UNUSED(shm);
    Q_UNUSED(screen);
}

#include "moc_screenshooter.cpp"
