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

#include <QtCore/QStandardPaths>
#include <QtCore/QTemporaryFile>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include "screenshooter.h"
#include "screenshooter_p.h"
#include "screenshot.h"

#include <wayland-client.h>

//#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * ScreenshooterPrivate
 */

ScreenshooterPrivate::ScreenshooterPrivate()
    : QtWayland::greenisland_screenshooter()
    , shm(Q_NULLPTR)
{
}

Screenshot *ScreenshooterPrivate::shoot(Screenshooter::What what, QScreen *screen)
{
    Q_Q(Screenshooter);

    int w = screen->size().width();
    int h = screen->size().height();
    int stride = w * 4;
    int area = w * h;

    const QString shmFileTemplate = QStringLiteral("%1/hawaii-screenshooter-shm-XXXXXX")
            .arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    QTemporaryFile tmpFile(shmFileTemplate);
    if (!tmpFile.open()) {
        qWarning("Failed to create temporary file for shm buffer: %s",
                 qPrintable(tmpFile.errorString()));
        return Q_NULLPTR;
    }

    int fd = tmpFile.handle();
    int flags = fcntl(fd, F_GETFD);
    if (flags != -1)
        fcntl(fd, F_SETFD, flags | FD_CLOEXEC);

    if (ftruncate(fd, area) < 0) {
        qWarning("ftruncate failed: %s", strerror(errno));
        return Q_NULLPTR;
    }

    uchar *data = static_cast<uchar *>(mmap(Q_NULLPTR, area, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    if (data == static_cast<uchar *>(MAP_FAILED)) {
        qWarning("Failed to allocate memory for the screenshot: %s",
                 qPrintable(strerror(errno)));
        return Q_NULLPTR;
    }

    wl_shm_pool *pool = wl_shm_create_pool(shm, fd, area);
    wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0, w, h, stride, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);

    tmpFile.close();

    return new Screenshot(what, screen, data, buffer, q);
}

/*
 * Screenshooter
 */

Screenshooter::Screenshooter(QObject *parent)
    : QObject(*new ScreenshooterPrivate(), parent)
{
}

void Screenshooter::takeScreenshot(What what)
{
    Q_D(Screenshooter);

    Q_FOREACH (QScreen *screen, QGuiApplication::screens()) {
        Screenshot *ss = d->shoot(what, screen);
        d->pendingScreenshots.append(ss);
    }
}

#include "moc_screenshooter.cpp"
