/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 * Copyright (C) 2015 The Qt Company Ltd.
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
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

#pragma once

#include <QtGui/QPalette>
#include <qpa/qplatformtheme.h>

class ResourceHelper
{
public:
    ResourceHelper();
    ~ResourceHelper() { clear(); }

    void clear();

    static void readPalette(const QString &fileName, QPalette *pal);
    static bool readColor(QPalette *pal, QPalette::ColorRole role, const QVariant &value);

    QPalette *palettes[QPlatformTheme::NPalettes];
    QFont *fonts[QPlatformTheme::NFonts];
};
