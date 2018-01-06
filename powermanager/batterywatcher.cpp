/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <Solid/Device>
#include <Solid/DeviceNotifier>
#include <Solid/Battery>

#include <LiriNotifications/Notification>

#include "batterywatcher.h"

BatteryWatcher::BatteryWatcher(QObject *parent)
    : QObject(parent)
{
    // Connect existing batteries
    QList<Solid::Device> devices = Solid::Device::listFromType(Solid::DeviceInterface::Battery, QString());
    for (Solid::Device &device : devices) {
        Solid::Battery *battery = device.as<Solid::Battery>();
        connect(battery, &Solid::Battery::energyChanged, this, &BatteryWatcher::batteryChanged);
        connect(battery, &Solid::Battery::chargeStateChanged, this, &BatteryWatcher::batteryChanged);
    }

    // Handle batteries as they come and go
    Solid::DeviceNotifier *notifier = Solid::DeviceNotifier::instance();
    connect(notifier, &Solid::DeviceNotifier::deviceAdded, [this](const QString &udi) {
        Solid::Device device(udi);
        if (device.as<Solid::Battery>()) {
            Solid::Battery *battery = device.as<Solid::Battery>();
            connect(battery, &Solid::Battery::energyChanged, this, &BatteryWatcher::batteryChanged);
            connect(battery, &Solid::Battery::chargeStateChanged, this, &BatteryWatcher::batteryChanged);
        }
    });
    connect(notifier, &Solid::DeviceNotifier::deviceRemoved, [this](const QString &udi) {
        Solid::Device device(udi);
        if (device.as<Solid::Battery>()) {
            Solid::Battery *battery = device.as<Solid::Battery>();
            disconnect(battery, &Solid::Battery::energyChanged, this, &BatteryWatcher::batteryChanged);
            disconnect(battery, &Solid::Battery::chargeStateChanged, this, &BatteryWatcher::batteryChanged);
        }
    });
}

void BatteryWatcher::batteryChanged()
{
    Solid::Battery *battery = qobject_cast<Solid::Battery *>(sender());
    if (!battery)
        return;

    // We take action only when the battery is discharging
    if (battery->chargeState() != Solid::Battery::Discharging)
        return;

    // Skip unknown devices
    if (battery->type() == Solid::Battery::UnknownBattery)
        return;

    // UPS discharging
    if (battery->type() == Solid::Battery::UpsBattery) {
        Liri::Notification *notification = new Liri::Notification(this);
        //: Low battery notification application name
        notification->setApplicationName(tr("Power"));
        notification->setUrgency(Liri::Notification::UrgencyCritical);

        //: UPS discharging notification summary
        notification->setSummary(tr("UPS Discharging"));

        if (battery->timeToEmpty() >= 0) {
            const quint64 timeToEmpty = static_cast<quint64>(battery->timeToEmpty()) * 1000;
            const QString timeToEmptyString = m_formatter.formatDuration(timeToEmpty, KFormat::HideSeconds);
            //: UPS discharging notification body
            notification->setBody(tr("%1 of UPS backup power remaining (%2%)").arg(timeToEmpty).arg(battery->chargePercent()));
        } else {
            //: UPS discharging notification body
            notification->setBody(tr("Unkown amount of UPS backup power remaining"));
        }

        notification->setTimeout(30000);
        notification->setHint(QStringLiteral("transient"), true);
        notification->send();
    }

    // Number of batteries
    int numBatteries = Solid::Device::listFromType(Solid::DeviceInterface::Battery, QString()).count();

    // Determine battery level
    double level = 100 * battery->energy() / battery->energyFull();
    if (level < 15) {
        Liri::Notification *notification = new Liri::Notification(this);
        //: Low battery notification application name
        notification->setApplicationName(tr("Power"));
        notification->setUrgency(Liri::Notification::UrgencyCritical);

        if (level < 5) {
            // Critical level
            switch (battery->type()) {
            case Solid::Battery::PrimaryBattery:
                if (numBatteries > 1) {
                    //: Laptop battery critically low, and we have more than one battery of that kind
                    notification->setSummary(tr("Laptop battery critically low"));
                } else {
                    //: Laptop battery critically low, and we have only one of that kind
                    notification->setSummary(tr("Battery critically low"));
                }

                // TODO: Replace 'will shutdown' depending on the action (hibernate, suspend, ...)
                //: Tell users what will happen if the computer won't be charged soon enough
                notification->setBody(tr("The system will shutdown very soon unless it is plugged in"));
                break;
            case Solid::Battery::UpsBattery:
                //: UPS battery level is critically low
                notification->setSummary(tr("UPS low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Approximately %1% of remaining UPS backup power. "
                                         "Restore AC power to your system to avoid losing data.").arg(battery->chargePercent()));
                break;
            case Solid::Battery::PdaBattery:
                //: PDA battery level is critically low
                notification->setSummary(tr("PDA battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("PDA is very low in power (%1%). "
                                         "This device will soon stop functioning if not charged.").arg(battery->chargePercent()));
                break;
            case Solid::Battery::MouseBattery:
                //: Mouse battery level is critically low
                notification->setSummary(tr("Mouse battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Wireless mouse is very low in power (%1%). "
                                         "This device will soon stop functioning if not charged.").arg(battery->chargePercent()));
                break;
            case Solid::Battery::KeyboardBattery:
                //: Keyboard battery level is critically low
                notification->setSummary(tr("Keyboard battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Wireless keyboard is very low in power (%1%). "
                                         "This device will soon stop functioning if not charged.").arg(battery->chargePercent()));
                break;
            case Solid::Battery::KeyboardMouseBattery:
                //: Combined keyboard/mouse battery level is critically low
                notification->setSummary(tr("Combined keyboard and mouse battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Wireless combined keyboard and mouse is very low in power (%1%). "
                                         "This device will soon stop functioning if not charged.").arg(battery->chargePercent()));
                break;
            case Solid::Battery::CameraBattery:
                //: Camera battery level is critically low
                notification->setSummary(tr("Camera battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Camera is very low in power (%1%). "
                                         "This device will soon stop functioning if not charged.").arg(battery->chargePercent()));
                break;
            case Solid::Battery::PhoneBattery:
                //: Phone battery level is critically low
                notification->setSummary(tr("Cell phone battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Cell phone is very low in power (%1%). "
                                         "This device will soon stop functioning if not charged.").arg(battery->chargePercent()));
                break;
            case Solid::Battery::MonitorBattery:
                //: Monitor battery level is critically low
                notification->setSummary(tr("Monitor battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Monitor is very low in power (%1%). "
                                         "This device will soon stop functioning if not charged.").arg(battery->chargePercent()));
                break;
            case Solid::Battery::UnknownBattery:
                // Skipped
                break;
            }
        } else {
            // Low level
            switch (battery->type()) {
            case Solid::Battery::PrimaryBattery:
                if (numBatteries > 1) {
                    //: Laptop battery low, and we have more than one battery of that kind
                    notification->setSummary(tr("Laptop battery low"));
                } else {
                    //: Laptop battery low, and we have only one of that kind
                    notification->setSummary(tr("Battery low"));
                }

                if (battery->timeToEmpty() >= 0) {
                    const quint64 timeToEmpty = static_cast<quint64>(battery->timeToEmpty()) * 1000;
                    const QString timeToEmptyString = m_formatter.formatDuration(timeToEmpty, KFormat::HideSeconds);
                    //: Tell users how much power is left (time and percentage)
                    notification->setBody(tr("Approximately %1 remaining (%2%)").arg(timeToEmptyString).arg(battery->chargePercent()));
                } else {
                    //: Tell users how much power is left (percentage)
                    notification->setBody(tr("Battery low in power (%1%)").arg(battery->chargePercent()));
                }
                break;
            case Solid::Battery::UpsBattery:
                //: UPS battery level is low
                notification->setSummary(tr("UPS low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Approximately %1% of remaining UPS backup power").arg(battery->chargePercent()));
                break;
            case Solid::Battery::PdaBattery:
                //: PDA battery level is low
                notification->setSummary(tr("PDA battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("PDA is low in power (%1%)").arg(battery->chargePercent()));
                break;
            case Solid::Battery::MouseBattery:
                //: Mouse battery level is low
                notification->setSummary(tr("Mouse battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Wireless mouse is low in power (%1%)").arg(battery->chargePercent()));
                break;
            case Solid::Battery::KeyboardBattery:
                //: Keyboard battery level is low
                notification->setSummary(tr("Keyboard battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Wireless keyboard is low in power (%1%)").arg(battery->chargePercent()));
                break;
            case Solid::Battery::KeyboardMouseBattery:
                //: Combined keyboard/mouse battery level is low
                notification->setSummary(tr("Combined keyboard and mouse battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Wireless combined keyboard and mouse is low in power (%1%)").arg(battery->chargePercent()));
                break;
            case Solid::Battery::CameraBattery:
                //: Camera battery level is low
                notification->setSummary(tr("Camera battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Camera is low in power (%1%)").arg(battery->chargePercent()));
                break;
            case Solid::Battery::PhoneBattery:
                //: Phone battery level is low
                notification->setSummary(tr("Cell phone battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Cell phone is low in power (%1%)").arg(battery->chargePercent()));
                break;
            case Solid::Battery::MonitorBattery:
                //: Monitor battery level is low
                notification->setSummary(tr("Monitor battery low"));
                //: Tell users how much power is left (percentage)
                notification->setBody(tr("Monitor is low in power (%1%)").arg(battery->chargePercent()));
                break;
            case Solid::Battery::UnknownBattery:
                // Skipped
                break;
            }
        }

        notification->setTimeout(30000);
        notification->setHint(QStringLiteral("transient"), true);
        notification->send();
    }
}
