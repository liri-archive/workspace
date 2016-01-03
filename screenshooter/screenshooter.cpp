/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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

#include <QtCore/QDateTime>
#include <QtCore/QStandardPaths>
#include <QtCore/QTimer>
#include <QtGui/QGuiApplication>
#include <QtGui/QImage>
#include <QtGui/QScreen>
#include <QtQml/QQmlContext>

#include <GreenIsland/Client/Output>

#include "screenshooter.h"

/*
 * InteractiveStartupEvent
 */

static const QEvent::Type InteractiveStartupEventType =
        static_cast<QEvent::Type>(QEvent::registerEventType());

InteractiveStartupEvent::InteractiveStartupEvent()
    : QEvent(InteractiveStartupEventType)
{
}

/*
 * StartupEvent
 */

static const QEvent::Type StartupEventType =
        static_cast<QEvent::Type>(QEvent::registerEventType());

StartupEvent::StartupEvent(Screenshooter::What w, Client::Screenshooter::Effects e, int d)
    : QEvent(StartupEventType)
    , what(w)
    , effects(e)
    , delay(d)
{
}

/*
 * Screenshooter
 */

Screenshooter::Screenshooter(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
    , m_interactive(false)
    , m_inProgress(false)
    , m_engine(new QQmlApplicationEngine(this))
    , m_thread(new QThread())
    , m_connection(Client::ClientConnection::fromQt())
    , m_registry(new Client::Registry(this))
{
    // Wayland connection in a separate thread
    Q_ASSERT(m_connection);
    m_connection->moveToThread(m_thread);
    m_thread->start();
}

Screenshooter::~Screenshooter()
{
    delete m_shooter;
    delete m_shm;
    delete m_registry;
    delete m_connection;

    m_thread->quit();
    m_thread->wait();
}

bool Screenshooter::event(QEvent *event)
{
    if (event->type() == InteractiveStartupEventType) {
        m_interactive = true;
        initialize();
        return true;
    } else if (event->type() == StartupEventType) {
        StartupEvent *e = static_cast<StartupEvent *>(event);
        m_cliOptions.what = e->what;
        m_cliOptions.pointer = e->effects.testFlag(Client::Screenshooter::EffectPointer);
        m_cliOptions.border = e->effects.testFlag(Client::Screenshooter::EffectBorder);
        m_cliOptions.delay = e->delay;
        initialize();
        return true;
    }

    return QObject::event(event);
}

void Screenshooter::takeScreenshot(What what, bool includePointer, bool includeBorder)
{
    if (m_inProgress) {
        qWarning("Cannot take another screenshot while a previous capture is in progress");
        return;
    }

    Client::Screenshooter::Effects effects;
    if (includePointer)
        effects &= Client::Screenshooter::EffectPointer;
    if (includeBorder)
        effects &= Client::Screenshooter::EffectBorder;

    Client::Screenshot *ss;

    switch (what) {
    case Screen:
        Q_FOREACH (QScreen *screen, QGuiApplication::screens()) {
            ss = m_shooter->captureOutput(Client::Output::fromQt(screen, this), effects);
            m_pending.append({screen->geometry().topLeft(), ss, Q_NULLPTR});
            setupScreenshot(ss);
        }
        break;
    case ActiveWindow:
        ss = m_shooter->captureActiveWindow(effects);
        m_pending.append({QPoint(0, 0), ss, Q_NULLPTR});
        setupScreenshot(ss);
        break;
    case Window:
        ss = m_shooter->captureWindow(effects);
        m_pending.append({QPoint(0, 0), ss, Q_NULLPTR});
        setupScreenshot(ss);
        break;
    case Area:
        ss = m_shooter->captureArea(effects);
        m_pending.append({QPoint(0, 0), ss, Q_NULLPTR});
        setupScreenshot(ss);
        break;
    }

    m_inProgress = true;
}

void Screenshooter::initialize()
{
    if (m_initialized)
        return;

    // Interfaces
    connect(m_registry, &Client::Registry::interfacesAnnounced,
            this, &Screenshooter::interfacesAnnounced);
    connect(m_registry, &Client::Registry::interfaceAnnounced,
            this, &Screenshooter::interfaceAnnounced);

    // Setup registry
    m_registry->create(m_connection->display());
    m_registry->setup();

    m_initialized = true;
}

void Screenshooter::process()
{
    int minX = INT_MAX, minY = INT_MAX;
    int width = 0, height = 0;

    // Calculate the final size
    Q_FOREACH (const ScreenshotRequest &sr, m_process) {
        minX = qMin(minX, sr.position.x());
        minY = qMin(minY, sr.position.y());
        width += sr.buffer->size().width();
        if (sr.buffer->size().height() > height)
            height = sr.buffer->size().height();
    }

    // Calculate the new stride and allocate data
    int stride = width * 4;
    uchar *data = new uchar[stride * height];
    memset(data, 0x00, stride * height);

    // Compose a new image
    Q_FOREACH (const ScreenshotRequest &sr, m_process) {
        // Copy buffer data to the image
        uchar *address = sr.buffer->address();
        uchar *newdata = data + (sr.position.y() - minY) * stride + (sr.position.x() - minX) * 4;
        for (int i = 0; i < sr.buffer->size().height(); i++) {
            memcpy(newdata, address, sr.buffer->stride());
            newdata += stride;
            address += sr.buffer->stride();
        }

        // Fill empty space with black
        for (int i = sr.buffer->size().height(); i < height; ++i) {
            for (uchar *line = newdata + 3; line < newdata + sr.buffer->stride(); line += 4)
                *line = 0xff;
            newdata += stride;
        }
    }

    // Create an image
    QImage image(data, width, height, stride, QImage::Format_RGB32);
    image.save(screenshotFileName());

    // Delete data and get ready for another screenshot
    delete []data;
    m_inProgress = false;
    Q_EMIT screenshotDone();

    // Quit in non-interactive mode because we only take one screenshot
    if (!m_interactive)
        QGuiApplication::quit();
}

void Screenshooter::setupScreenshot(Client::Screenshot *screenshot)
{
    connect(screenshot, &Client::Screenshot::setupFailed, this, [this] {
        qCritical("Failed to setup buffer for recording");
        QGuiApplication::quit();
    });
    connect(screenshot, &Client::Screenshot::done, this,
            [this, screenshot](Client::Buffer *buffer) {
        // Move to the buffers vector for processing
        for (int i = 0; i < m_pending.count(); i++) {
            ScreenshotRequest sr = m_pending.at(i);
            if (sr.screenshot == screenshot) {
                m_pending.takeAt(i);
                sr.buffer = buffer;
                m_process.append(sr);
                break;
            }
        }

        // Process images if there are no more pending screenshots
        if (m_pending.isEmpty())
            process();
    });
    connect(screenshot, &Client::Screenshot::failed, this,
            [this](Client::Screenshot::Error error) {
        switch (error) {
        case Client::Screenshot::ErrorBadBuffer:
            qCritical("Failed to record a frame: bad buffer");
            QGuiApplication::quit();
            break;
        }
    });
}

QString Screenshooter::screenshotFileName() const
{
    return QStringLiteral("%1/%2.png")
            .arg(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))
            .arg(tr("Screenshot from %1").arg(QDateTime::currentDateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"))));
}

void Screenshooter::interfacesAnnounced()
{
    if (!m_shm)
        qCritical("Unable to create shared memory buffers");

    if (!m_shooter)
        qCritical("Wayland compositor doesn't have screenshooter capabilities");

    if (m_interactive) {
        m_engine->rootContext()->setContextProperty(QLatin1String("Screenshooter"), this);
        m_engine->load(QUrl(QLatin1String("qrc:/qml/main.qml")));
    } else {
        QTimer::singleShot(m_cliOptions.delay * 1000, this, [this] {
            takeScreenshot(m_cliOptions.what, m_cliOptions.pointer, m_cliOptions.border);
        });
    }
}

void Screenshooter::interfaceAnnounced(const QByteArray &interface,
                                       quint32 name, quint32 version)
{
    if (interface == Client::Shm::interfaceName())
        m_shm = m_registry->createShm(name, version);
    else if (interface == Client::Screenshooter::interfaceName())
        m_shooter = m_registry->createScreenshooter(m_shm, name, version);
}

#include "moc_screenshooter.cpp"
