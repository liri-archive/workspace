/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef HAWAIITHEME_P_H
#define HAWAIITHEME_P_H

#include <qpa/qplatformtheme_p.h>

#include <Hawaii/Settings/QGSettings>

#include "hawaiitheme.h"
#include "resourcehelper.h"

class HintsSettings;

class HawaiiThemePrivate : public QPlatformThemePrivate
{
public:
    HawaiiThemePrivate();
    ~HawaiiThemePrivate();

    void refresh();

    ResourceHelper resources;
    Hawaii::QGSettings *settings;
    HintsSettings *hints;
};

#endif // HAWAIITHEME_P_H
