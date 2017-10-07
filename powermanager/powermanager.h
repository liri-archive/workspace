/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#pragma once

#include <QObject>

#include <Qt5GSettings/QGSettings>

class PowerManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int sleepInactiveAcTimeout READ sleepInactiveAcTimeout NOTIFY sleepInactiveAcTimeoutChanged)
    Q_PROPERTY(int sleepInactiveBatteryTimeout READ sleepInactiveBatteryTimeout NOTIFY sleepInactiveBatteryTimeoutChanged)
public:
    explicit PowerManager(QObject *parent = nullptr);

    int sleepInactiveAcTimeout() const;
    int sleepInactiveBatteryTimeout() const;

Q_SIGNALS:
    void sleepInactiveAcTimeoutChanged();
    void sleepInactiveAcTypeChanged();
    void sleepInactiveBatteryTimeoutChanged();
    void sleepInactiveBatteryTypeChanged();

private:
    QtGSettings::QGSettings *m_settings = nullptr;
    int m_sleepAcTimeout = 0;
    QString m_sleepAcAction;
    int m_sleepBatteryTimeout = 0;
    QString m_sleepBatteryAction;

private Q_SLOTS:
    void settingChanged(const QString &key);
};
