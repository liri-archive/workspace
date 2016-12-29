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

#ifndef SCREENSHOOTER_H
#define SCREENSHOOTER_H

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QVector>
#include <QtQml/QQmlApplicationEngine>

#include <Liri/WaylandClient/ClientConnection>
#include <Liri/WaylandClient/Registry>
#include <Liri/WaylandClient/Screenshooter>
#include <Liri/WaylandClient/Shm>

#include "imageprovider.h"

using namespace Liri;

class Screenshooter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled NOTIFY enabledChanged)
public:
    enum What {
        Screen = 1,
        ActiveWindow,
        Window,
        Area
    };
    Q_ENUM(What)

    Screenshooter(QObject *parent = Q_NULLPTR);
    ~Screenshooter();

    bool isEnabled() const;

    Q_INVOKABLE void takeScreenshot(What what, bool includePointer, bool includeBorder);
    Q_INVOKABLE QString screenshotFileName() const;
    Q_INVOKABLE void saveScreenshot(const QUrl &fileName);

Q_SIGNALS:
    void enabledChanged();
    void screenshotDone();

protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_initialized;
    bool m_interactive;
    bool m_inProgress;
    QQmlApplicationEngine *m_engine;
    QThread *m_thread;
    WaylandClient::ClientConnection *m_connection;
    WaylandClient::Registry *m_registry;
    WaylandClient::Shm *m_shm;
    WaylandClient::Screenshooter *m_shooter;

    struct {
        bool initialized;
        quint32 name;
        quint32 version;
    } m_deferredShooter;

    struct ScreenshotRequest {
        QPoint position;
        WaylandClient::Screenshot *screenshot;
        WaylandClient::Buffer *buffer;
    };

    QVector<ScreenshotRequest> m_pending;
    QVector<ScreenshotRequest> m_process;

    struct {
        What what;
        bool pointer;
        bool border;
        int delay;
    } m_cliOptions;

    ImageProvider *m_imageProvider;

    void initialize();
    void process();
    void setupScreenshot(WaylandClient::Screenshot *screenshot);

private Q_SLOTS:
    void interfacesAnnounced();
    void interfaceAnnounced(const QByteArray &interface, quint32 name, quint32 version);
};

class InteractiveStartupEvent : public QEvent
{
public:
    InteractiveStartupEvent();
};

class StartupEvent : public QEvent
{
public:
    StartupEvent(Screenshooter::What what, WaylandClient::Screenshooter::Effects effects, int delay);

    Screenshooter::What what;
    WaylandClient::Screenshooter::Effects effects;
    int delay;
};

#endif // SCREENSHOOTER_H
