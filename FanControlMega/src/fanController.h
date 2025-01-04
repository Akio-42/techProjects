#ifndef FANCONTROLLER_H
#define FANCONTROLLER_H

#include <Arduino.h>
#include <fan.h>

class FanController {
private:
    FanController() {};

    static int fan1RpmInterruptPin;
    static int fan2RpmInterruptPin;
    static int fan3RpmInterruptPin;

    // this array holds the objects with the settings of the fans. 
    //Note that the first fan is a dummy object so we can nicely access fan 1-3 on position 1-3 of the array.
    static Fan fans[4];

    // the time interval used for pwm time calculation
    static unsigned long rpmCalculationTime;

    // variable to compare the passed time to
    static unsigned long now;

    // rpmCycleTime in seconds
    static double rpmCycleTime;

    // 25kHz pwm 
    static const unsigned char OCRCompareValue = 79;  // The timer will count up to this value and then reset. This value is chosen to get 25 kHz signal together with the prescaler for the timer configured to 8
    static const unsigned char maxFanSpeed = 78;  // this value sets the maximum dutiy cycle. It must be at maximum OCRCompareValue -1. If not the timer will run up and never find a match. No pwm singal will be output on the pin in this case. 
    //static const unsigned char minFanSpeed = 0;

    // helper function
    static void countRpmSpeedFan1();
    static void countRpmSpeedFan2();
    static void countRpmSpeedFan3();

    static unsigned char mapFanSpeedPercentToPwmValue(int fanSpeedPercent);

    static void setFanSpeedInternal(unsigned char fanSpeed);

public:
    // code called during setup
    static void init();

    // set fan speed
    static void setFanSpeed();

    // calculate fan rpm values
    static void calculateRpmSpeeds();

    // save fan states to eeprom
    static void saveFanSpeed();

    // returns an pointer to the fan object so it can be used in the controller to display the fan values and adjust the fan settings
    static Fan* getFan(int id);
};

#endif // FANCONTROLLER_H