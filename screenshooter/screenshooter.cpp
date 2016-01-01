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
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtQml/QQmlContext>

#include <GreenIsland/Client/Output>

#include "screenshooter.h"

static const QEvent::Type StartupEventType =
        static_cast<QEvent::Type>(QEvent::registerEventType());

/*
 * StartupEvent
 */

StartupEvent::StartupEvent()
    : QEvent(StartupEventType)
{
}

/*
 * Screenshooter
 */

Screenshooter::Screenshooter(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
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
    if (event->type() == StartupEventType) {
        initialize();
        return true;
    }

    return QObject::event(event);
}

void Screenshooter::takeScreenshot(What what)
{
    if (m_inProgress) {
        qWarning("Cannot take another screenshot while a previous capture is in progress");
        return;
    }

    Client::Screenshot *ss;

    switch (what) {
    case Screen:
        Q_FOREACH (QScreen *screen, QGuiApplication::screens()) {
            ss = m_shooter->captureOutput(Client::Output::fromQt(screen, this));
            m_pending.append({ss, screen, QImage()});
            setupScreenshot(ss);
        }
        break;
    case ActiveWindow:
        ss = m_shooter->captureActiveWindow();
        m_pending.append({ss, Q_NULLPTR, QImage()});
        setupScreenshot(ss);
        break;
    case Window:
        ss = m_shooter->captureWindow();
        m_pending.append({ss, Q_NULLPTR, QImage()});
        setupScreenshot(ss);
        break;
    case Area:
        ss = m_shooter->captureArea();
        m_pending.append({ss, Q_NULLPTR, QImage()});
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
    Q_FOREACH (const ScreenshotRequest &sr, m_buffers) {
        int imageX = 0, imageY = 0, imageWidth = 0, imageHeight = 0;

        if (sr.screenshot->captureType() == Client::Screenshot::CaptureOutput) {
            imageX = sr.screen->geometry().x();
            imageY = sr.screen->geometry().y();
            imageWidth = sr.screen->size().width();
            if (sr.screen->size().height() > height)
                imageHeight = sr.screen->size().height();
        } else {
            imageX = sr.image.width();
            imageY = sr.image.height();
            imageWidth = imageX;
            if (sr.image.height() > height)
                imageHeight = sr.image.height();
        }

        minX = qMin(minX, imageX);
        minY = qMin(minY, imageY);
        width += imageWidth;
        if (imageHeight > 0)
            height = imageHeight;
    }

    // Calculate the new stride
    int stride = width * 4;

    // Compose a new image
    uchar *data = new uchar[stride * height];
    memset(data, 0x00, stride * height);
    Q_FOREACH (const ScreenshotRequest &sr, m_buffers) {
        // Determine height and coordinates
        int imageHeight = sr.image.height();
        int imageX = 0, imageY = 0;
        int imageStride = sr.image.bytesPerLine();
        if (sr.screenshot->captureType() == Client::Screenshot::CaptureOutput) {
            imageX = sr.screen->geometry().x();
            imageY = sr.screen->geometry().y();
        }

        // Copy buffer data to the image
        uchar *address = const_cast<uchar *>(sr.image.bits());
        uchar *newdata = data + (imageY - minY) * imageStride * (imageX - minX) * 4;
        for (int i = 0; i < imageHeight; i++) {
            memcpy(newdata, address, imageStride);
            newdata += imageStride;
            address += imageStride;
        }

        // Fill empty space with black
        for (int i = imageHeight; i < height; i++) {
            for (uchar *line = newdata + 3; line < newdata + imageStride; line += 4)
                *line = 0xff;
            newdata += imageStride;
        }
    }

    // Create an image
    QImage image(data, width, height, stride, QImage::Format_RGB32);
    image.save(QStringLiteral("%1/%2.png")
               .arg(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))
               .arg(tr("Screenshot from %1").arg(QDateTime::currentDateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss")))));

    // Delete data and get ready for another screenshot
    delete []data;
    m_inProgress = false;
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
                ScreenshotRequest req = m_pending.takeAt(i);
                req.image = buffer->image();
                m_buffers.append(req);
                delete buffer;
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

void Screenshooter::interfacesAnnounced()
{
    if (!m_shm)
        qCritical("Unable to create shared memory buffers");

    if (!m_shooter)
        qCritical("Wayland compositor doesn't have screenshooter capabilities");

    m_engine->rootContext()->setContextProperty(QLatin1String("Screenshooter"), this);
    m_engine->load(QUrl(QLatin1String("qrc:/qml/main.qml")));
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
