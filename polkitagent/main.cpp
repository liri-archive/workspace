/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
#include <QtQuick/QQuickWindow>
#include <QtQuickControls2/QQuickStyle>

#include "config.h"
#include "policykitagent.h"

int main(int argc, char *argv[])
{
    // HighDpi scaling
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // Setup application
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("PolicyKit Agent"));
    app.setApplicationVersion(QStringLiteral(HAWAII_WORKSPACE_VERSION));
    app.setOrganizationDomain(QStringLiteral("hawaiios.org"));
    app.setOrganizationName(QStringLiteral("Hawaii"));
    app.setFallbackSessionManagementEnabled(false);
    app.setDesktopFileName(QStringLiteral("org.hawaiios.PolkitAgent.desktop"));
    app.setQuitOnLastWindowClosed(false);

    // Set default style
    if (QQuickStyle::name().isEmpty())
        QQuickStyle::setStyle(QLatin1String("Material"));

    // For transparent windows
    QQuickWindow::setDefaultAlphaBuffer(true);

    // Create the agent instance
    PolicyKitAgent::instance();

    return app.exec();
}
