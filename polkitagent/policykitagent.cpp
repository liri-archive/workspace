/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>

#include "config.h"
#include "policykitagent.h"
#include "policykitagent_p.h"

#include <PolkitQt1/Subject>

#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

#if HAVE_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif

Q_GLOBAL_STATIC(PolicyKitAgent, s_agent)

/*
 * PolicyKitAgentPrivate
 */

PolicyKitAgentPrivate::PolicyKitAgentPrivate(PolicyKitAgent *self)
    : progressing(false)
    , canceled(false)
    , session(Q_NULLPTR)
    , q_ptr(self)
{
    // Load QML window
    engine = new QQmlApplicationEngine(QUrl("qrc:/qml/authdialog.qml"), self);
    window = engine->rootObjects().at(0);
}

void PolicyKitAgentPrivate::createDialog(const QString &actionId,
                                         const QString &message,
                                         const QString &iconName,
                                         const QString &realName,
                                         const QString &avatar)
{
    Q_Q(PolicyKitAgent);

    // Properties
    window->setProperty("actionId", actionId);
    window->setProperty("message", message);
    window->setProperty("iconName", iconName);
    window->setProperty("realName", realName);
    window->setProperty("avatar", avatar);

    // Connect signals
    QObject::connect(window, SIGNAL(authenticationCanceled()),
                     q, SLOT(abortAuthentication()));
    QObject::connect(window, SIGNAL(authenticationReady(QString)),
                     q, SLOT(authenticate(QString)));
}

void PolicyKitAgentPrivate::hideDialog()
{
    if (!window)
        return;

    QMetaObject::invokeMethod(window, "close", Qt::QueuedConnection);
}

/*
 * PolicyKitAgent
 */

PolicyKitAgent::PolicyKitAgent(QObject *parent)
    : PolkitQt1::Agent::Listener(parent)
    , d_ptr(new PolicyKitAgentPrivate(this))
{
    PolkitQt1::UnixSessionSubject session(
                QCoreApplication::instance()->applicationPid());
    if (registerListener(session, "/org/hawaii/PolicyKit1/AuthenticationAgent")) {
        qDebug() << "PolicyKit agent started!";
#if HAVE_SYSTEMD
        sd_notifyf(0,
                   "READY=1\n"
                   "STATUS=PolicyKit agent started\n"
                   "MAINPID=%llu",
                   QCoreApplication::applicationPid());
#endif
    } else {
        const char *msg = "Failed to register PolicyKit agent";
#if HAVE_SYSTEMD
        sd_notifyf(0,
                   "STATUS=%s\n"
                   "ERRNO=%i",
                   msg, EFAULT);
#endif
        qWarning() << msg;
    }
}

PolicyKitAgent::~PolicyKitAgent()
{
    delete d_ptr;
}

PolicyKitAgent *PolicyKitAgent::instance()
{
    return s_agent();
}

void PolicyKitAgent::initiateAuthentication(const QString &actionId,
                                            const QString &message,
                                            const QString &iconName,
                                            const PolkitQt1::Details &details,
                                            const QString &cookie,
                                            const PolkitQt1::Identity::List &identities,
                                            PolkitQt1::Agent::AsyncResult *result)
{
    Q_D(PolicyKitAgent);

    // Can't continue if another authorization is in progress
    if (d->progressing) {
        qDebug() << "Another authorization is in progress...";
        return;
    }

    // Reset variables to keep track of this authorization session
    d->progressing = true;
    d->canceled = false;
    d->session = Q_NULLPTR;

    // Cache cookie: we might need to reused it (to athenticated again
    // upon failure)
    d->cookie = cookie;

    // We don't support more than one identity
    if (identities.size() > 1)
        qDebug() << "PolicyKitAgent: received" << identities.size()
                 << "identities, only considering one";

    // uid to authenticate
    uid_t uidToAuthenticate = ::geteuid();
    struct passwd *pw = Q_NULLPTR;

    // Find the right username
    bool found = false;
    for (int i = 0; i < identities.size(); i++) {
        PolkitQt1::Identity identity = identities.at(i);

        if (identity.toUnixUserIdentity().uid() == ::geteuid()) {
            found = true;
            pw = ::getpwuid(::geteuid());
            d->realName = QLatin1String(pw->pw_gecos);
            uidToAuthenticate = identity.toUnixUserIdentity().uid();
            break;
        }
    }

    // Can we find root?
    found = false;
    pw = ::getpwnam("root");
    for (int i = 0; i < identities.size(); i++) {
        PolkitQt1::Identity identity = identities.at(i);

        if (identity.toUnixUserIdentity().uid() == pw->pw_uid) {
            found = true;
            d->realName = tr("Administrator");
            uidToAuthenticate = identity.toUnixUserIdentity().uid();
            break;
        }
    }
    if (!found) {
        PolkitQt1::Identity identity = identities.at(0);
        uidToAuthenticate = identity.toUnixUserIdentity().uid();
        pw = ::getpwuid(uidToAuthenticate);
        d->realName = QLatin1String(pw->pw_gecos);
    }

    // Identity to authenticate
    d->identity = PolkitQt1::UnixUserIdentity(uidToAuthenticate);

    // Associate sessions with identities and initiate session
    PolkitQt1::Agent::Session *session =
            new PolkitQt1::Agent::Session(d->identity, d->cookie, result);
    connect(session, SIGNAL(request(QString, bool)), this, SLOT(request(QString, bool)));
    connect(session, SIGNAL(completed(bool)), this, SLOT(completed(bool)));
    connect(session, SIGNAL(showInfo(QString)), this, SLOT(showInfo(QString)));
    connect(session, SIGNAL(showError(QString)), this, SLOT(showError(QString)));
    session->initiate();

    // Initiate authentication sequence
    // TODO: Find user icon
    d->createDialog(actionId, message, iconName, d->realName, "");
}

bool PolicyKitAgent::initiateAuthenticationFinish()
{
    Q_D(PolicyKitAgent);

    if (!d->progressing) {
        // Inform the dialog that the authentication sequence has finished
        Q_ASSERT(d->window);
        QMetaObject::invokeMethod(d->window, "authenticationFinished",
                                  Qt::QueuedConnection);
    }

    return true;
}

void PolicyKitAgent::cancelAuthentication()
{
    Q_D(PolicyKitAgent);

    qDebug() << "Canceling authentication...";

    // Close current session
    d->progressing = false;
    d->canceled = true;
    d->session = Q_NULLPTR;

    // Hide the dialog
    d->hideDialog();
}

void PolicyKitAgent::request(const QString &request, bool echo)
{
    Q_D(PolicyKitAgent);

    // We also need a session, which is the object that emits the signal
    PolkitQt1::Agent::Session *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    if (!session) {
        qWarning("The request() signal was emitted by an invalid session object!");
        return;
    }

    // Cache the session object because it will be used later when the
    // dialog will be either accepted or rejected
    d->session = session;

    // Normalize prompt
    QString prompt = request;
    if (request == QStringLiteral("Password:") || request == QStringLiteral("Password: "))
        prompt = tr("Password:");

    // Set the prompt and password echo
    Q_ASSERT(d->window);
    d->window->setProperty("prompt", prompt);
    d->window->setProperty("echo", echo);
    QMetaObject::invokeMethod(d->window, "open", Qt::QueuedConnection);
}

void PolicyKitAgent::completed(bool gainedAuthorization)
{
    Q_D(PolicyKitAgent);

    qDebug() << "Authorization complete - result:" << gainedAuthorization
             << "canceled:" << d->canceled;

    // Cache async result, will be reused in case of retry
    PolkitQt1::Agent::AsyncResult *result = d->session->result();

    if (gainedAuthorization) {
        // Authorization completed, hide dialog
        d->hideDialog();
    } else {
        // Authorization failed
        qDebug() << "Authorization failed!";

        if (d->canceled) {
            // Inform the dialog the authentication was canceled by the user
            result->setCompleted();
            d->hideDialog();
        } else {
            // The user didn't cancel the dialog, this is an actual
            // authentication failure
            Q_ASSERT(d->window);
            d->window->setProperty("errorMessage",
                                   tr("Sorry, that didn't work. Please try again."));

            // Cancel current session
            disconnect(d->session, SIGNAL(request(QString, bool)),
                       this, SLOT(request(QString, bool)));
            disconnect(d->session, SIGNAL(completed(bool)),
                       this, SLOT(completed(bool)));
            disconnect(d->session, SIGNAL(showInfo(QString)),
                       this, SLOT(showInfo(QString)));
            disconnect(d->session, SIGNAL(showError(QString)),
                       this, SLOT(showError(QString)));
            d->session->cancel();
        }

        d->session->deleteLater();
        d->session = Q_NULLPTR;
    }

    // Complete session only if authorization went fine, otherwise
    // we keep the dialog open and wait for the user to type a good password
    if (gainedAuthorization && !d->canceled) {
        // Unset variables that keep track of the authorization session
        d->progressing = false;

        // Complete session
        d->session->result()->setCompleted();
        d->session->deleteLater();
        d->session = Q_NULLPTR;
    }

    // Try again if there was an authentication error
    if (!gainedAuthorization && !d->canceled) {
        d->session = new PolkitQt1::Agent::Session(d->identity, d->cookie, result);
        connect(d->session, SIGNAL(request(QString, bool)),
                this, SLOT(request(QString, bool)));
        connect(d->session, SIGNAL(completed(bool)),
                this, SLOT(completed(bool)));
        connect(d->session, SIGNAL(showInfo(QString)),
                this, SLOT(showInfo(QString)));
        connect(d->session, SIGNAL(showError(QString)),
                this, SLOT(showError(QString)));
        d->session->initiate();
    }
}

void PolicyKitAgent::showInfo(const QString &text)
{
    Q_D(PolicyKitAgent);
    Q_ASSERT(d->window);
    d->window->setProperty("infoMessage", text);
}

void PolicyKitAgent::showError(const QString &text)
{
    Q_D(PolicyKitAgent);
    Q_ASSERT(d->window);
    d->window->setProperty("errorMessage", text);
}

void PolicyKitAgent::authenticate(const QString &response)
{
    Q_D(PolicyKitAgent);

    Q_ASSERT(d->session);
    d->canceled = false;
    d->session->setResponse(response);
}

void PolicyKitAgent::abortAuthentication()
{
    Q_D(PolicyKitAgent);

    qDebug() << "Aborting authentication...";

    // Delete session and inform the dialog that the
    // authentication was canceled so it can be hidden
    d->progressing = false;
    d->canceled = true;
    if (d->session)
        d->session->cancel();
}

#include "moc_policykitagent.cpp"
