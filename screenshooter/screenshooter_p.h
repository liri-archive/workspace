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

#ifndef SCREENSHOOTER_P_H
#define SCREENSHOOTER_P_H

#include <QtCore/private/qobject_p.h>

#include "qwayland-greenisland-screenshooter.h"

class ScreenshooterPrivate : public QObjectPrivate
                           , public QtWayland::greenisland_screenshooter
{
    Q_DECLARE_PUBLIC(Screenshooter)
public:
    ScreenshooterPrivate();
};

#endif // SCREENSHOOTER_P_H
