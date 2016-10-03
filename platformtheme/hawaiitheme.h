/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2010-2016 Pier Luigi Fiorini
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

#ifndef HAWAIITHEME_H
#define HAWAIITHEME_H

#include <QtCore/QHash>
#include <QtCore/QVariant>
#include <qpa/qplatformtheme.h>

class HawaiiThemePrivate;

class Q_GUI_EXPORT HawaiiTheme : public QPlatformTheme
{
    Q_DECLARE_PRIVATE(HawaiiTheme)
public:
    HawaiiTheme();

    bool usePlatformNativeDialog(DialogType type) const;
    QPlatformDialogHelper *createPlatformDialogHelper(DialogType type) const;

    const QPalette *palette(Palette type = SystemPalette) const;
    const QFont *font(Font type = SystemFont) const;

    QVariant themeHint(ThemeHint hint) const;
};

#endif // HAWAIITHEME_H
