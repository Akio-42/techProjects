#ifndef FAN_H
#define FAN_H

#include <Arduino.h>

class Fan {
private:
  int id;
  int speed;
  int rpm;
  int rpmCount;
  unsigned int timeIntervalStart;
  double rpmCycleTime;

public:
  // Constructor
  Fan(int fanId);

  // Getter for Id
  int getId() const;

  // Getter for Fan Speed
  int getSpeed() const;

  // Getter for Fan RPM
  int getRPM() const;

  // Get Formatted Name
  String getDisplayName() const;

  // Get formatted speed
  String getDisplaySpeed() const;

  // Get formatted speed
  String getDisplayRPM() const;

  // Set fan speed setting 0-100%
  void setSpeed(int newSpeed);

  // CountRpm impulse
  void countSpeedImpulse();

  // calculate RPM
  void calculateSpeed();

  // we  want to save the fan speed setting in the EEPROM so it is available after the controller was turned off an on again
  void saveSpeedToEeprom();
};

#endif // FAN_H