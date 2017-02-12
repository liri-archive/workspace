/****************************************************************************
 * This file is part of Liri.
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

#include <QtGui/QFont>
#include <QtQuickControls2/QQuickStyle>

#include "liritheme.h"
#include "liritheme_p.h"
#include "hintssettings.h"
#include "logging_p.h"

LiriTheme::LiriTheme()
    : QPlatformTheme(new LiriThemePrivate())
{
    qCDebug(gLcPlatformTheme) << "Initializing Liri platform theme";

    Q_D(LiriTheme);
    d->refresh();

    // Set Material style by default
    if (QQuickStyle::name().isEmpty())
        QQuickStyle::setStyle(QLatin1String("Material"));
}

bool LiriTheme::usePlatformNativeDialog(DialogType type) const
{
    Q_UNUSED(type);
    return false;
}

QPlatformDialogHelper *LiriTheme::createPlatformDialogHelper(DialogType type) const
{
    Q_UNUSED(type);
    return 0;
}

const QPalette *LiriTheme::palette(Palette type) const
{
    Q_D(const LiriTheme);
    return d->resources.palettes[type];
}

const QFont *LiriTheme::font(Font type) const
{
    Q_D(const LiriTheme);
    return d->resources.fonts[type];
}

QVariant LiriTheme::themeHint(ThemeHint hint) const
{
    Q_D(const LiriTheme);

    QVariant value = d->hints->themeHint(hint);
    if (value.isValid())
        return value;
    return QPlatformTheme::themeHint(hint);
}
