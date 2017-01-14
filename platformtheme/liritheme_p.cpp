/****************************************************************************
 * This file is part of Liri.
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

#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtGui/QFont>
#include <QtGui/QPalette>
#include <qpa/qplatformtheme.h>

#include "liritheme_p.h"
#include "hintssettings.h"

// Default fonts
static const char defaultSystemFontName[] = "Droid Sans";
static int defaultSystemFontSize = 11;
static const char defaultMonospaceFontName[] = "Droid Sans Mono";
static int defaultMonospaceFontSize = 11;

LiriThemePrivate::LiriThemePrivate()
{
    settings = new Vibe::QGSettings(QStringLiteral("io.liri.desktop.interface"),
                                      QStringLiteral("/io/liri/desktop/interface/"));
    hints = new HintsSettings(settings);
}

LiriThemePrivate::~LiriThemePrivate()
{
    hints->deleteLater();
    settings->deleteLater();
}

static QFont *readFont(const QString &family, int size)
{
    QFont font(family, size);
    return new QFont(font);
}

void LiriThemePrivate::refresh()
{
    resources.clear();

    // Load hints
    hints->collectHints();

    // Locate color scheme
    QString scheme = settings->value(QStringLiteral("colorScheme")).toString();
    QString schemeFileName = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                    QStringLiteral("color-schemes/%1.colors").arg(scheme));

    // Palette
    QPalette systemPalette = QPalette();
    ResourceHelper::readPalette(schemeFileName, &systemPalette);
    resources.palettes[QPlatformTheme::SystemPalette] = new QPalette(systemPalette);

    // Fonts
    QString fontFamily = settings->value(QStringLiteral("fontName")).toString();
    int fontSize = settings->value(QStringLiteral("fontSize")).toInt();
    QString monospaceFontFamily = settings->value(QStringLiteral("monospaceFontName")).toString();
    int monospaceFontSize = settings->value(QStringLiteral("monospaceFontSize")).toInt();
    QString smallFontFamily = settings->value(QStringLiteral("smallFontName")).toString();
    int smallFontSize = settings->value(QStringLiteral("smallFontSize")).toInt();
    QString miniFontFamily = settings->value(QStringLiteral("miniFontName")).toString();
    int miniFontSize = settings->value(QStringLiteral("miniFontSize")).toInt();

    // System font
    if (QFont *systemFont = readFont(fontFamily, fontSize))
        resources.fonts[QPlatformTheme::SystemFont] = systemFont;
    else
        resources.fonts[QPlatformTheme::SystemFont] = new QFont(QLatin1String(defaultSystemFontName), defaultSystemFontSize);

    // Monospace font
    if (QFont *monospaceFont = readFont(monospaceFontFamily, monospaceFontSize))
        resources.fonts[QPlatformTheme::FixedFont] = monospaceFont;
    else
        resources.fonts[QPlatformTheme::FixedFont] = new QFont(QLatin1String(defaultMonospaceFontName), defaultMonospaceFontSize);

    // Small font
    if (QFont *smallFont = readFont(smallFontFamily, smallFontSize))
        resources.fonts[QPlatformTheme::SmallFont] = smallFont;
    else
        resources.fonts[QPlatformTheme::SmallFont] = new QFont(QLatin1String(defaultSystemFontName), defaultSystemFontSize);

    // Mini font
    if (QFont *miniFont = readFont(miniFontFamily, miniFontSize))
        resources.fonts[QPlatformTheme::MiniFont] = miniFont;
    else
        resources.fonts[QPlatformTheme::MiniFont] = new QFont(QLatin1String(defaultSystemFontName), defaultSystemFontSize);

    // Other fonts
    QList<QPlatformTheme::Font> fonts;
    fonts << QPlatformTheme::TitleBarFont << QPlatformTheme::MdiSubWindowTitleFont
          << QPlatformTheme::DockWidgetTitleFont;
    Q_FOREACH (QPlatformTheme::Font font, fonts) {
        if (QFont *systemFont = readFont(fontFamily, fontSize))
            resources.fonts[font] = systemFont;
        else
            resources.fonts[font] = new QFont(QLatin1String(defaultSystemFontName), defaultSystemFontSize);
        resources.fonts[font]->setBold(true);
    }
}
