#include "fan.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <config.h>

Fan::Fan(int fanId) {
    id = fanId;
    speed = 0;
    setSpeed(EEPROM.read(id));
    rpm = 0;
    rpmCount = 0;
    timeIntervalStart = 0;
    rpmCycleTime = RPM_CYCLE_TIME;
}

int Fan::getId() const
{
    return id;
}

int Fan::getSpeed() const
{
    return speed;
}
int Fan::getRPM() const
{
    return rpm;
}
String Fan::getDisplayName() const
{
    return "Fan " + String(id);
}
String Fan::getDisplaySpeed() const
{
    return String(speed) + "%";
}
String Fan::getDisplayRPM() const
{
    return String(rpm);
}

void Fan::setSpeed(int newSpeed)
{
    if (speed != newSpeed) {
        if (newSpeed >= 0 && newSpeed <= 100)
        {
            speed = newSpeed;
        }
    }
}

void Fan::saveSpeedToEeprom()
{
    int eepromFanSpeed = EEPROM.read(id);
    if (speed != eepromFanSpeed) {
        EEPROM.write(id, speed);
    }
}

void Fan::countSpeedImpulse()
{
    rpmCount++;
}

void Fan::calculateSpeed()
{
    rpm = rpmCount * 30 / rpmCycleTime; // noctua fans give 2 pulses per revolution so we can multiply by 30 instead of 60 and divide through the rpmCycleTime.
    rpmCount = 0;

}
