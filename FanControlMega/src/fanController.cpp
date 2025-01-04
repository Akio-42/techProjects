#include "fancontroller.h"
#include <Arduino.h>
#include <fan.h>
#include <fanController.h>
#include <config.h>

int FanController::fan1RpmInterruptPin = FAN1_RPM_INTERRUPT_PIN;
int FanController::fan2RpmInterruptPin = FAN2_RPM_INTERRUPT_PIN;
int FanController::fan3RpmInterruptPin = FAN3_RPM_INTERRUPT_PIN;
unsigned long FanController::rpmCalculationTime = 0L;
unsigned long FanController::now = 0L;
int FanController::rpmCycleTimeMs = RPM_CYCLE_TIME * 1000;
Fan FanController::fans[4] = { Fan(0), Fan(1), Fan(2), Fan(3) };

void FanController::init() {

    // ------------- 25 kHz PWM and fan PWM pin definition -------------
    // Clear registers from any values
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1C = 0;


    // Set pwm mode to fast pwm with OCR1A as TOP of timer to enable to configure frequency -  wgm11, wgm10, wgm13, wgm12
    // set prescaler to 8 to get into the right frequency range for 25 kHz -  cs11
    // set OCR1B as compare match for the Compare Output Mode in fast PWM. This will allow us to set the duty cycle - com1b1
    TCCR1A = (1 << COM1B1) | (1 << WGM11) | (1 << WGM10);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    OCR1AH = 0;
    OCR1AL = OCRCompareValue;  // This results in a 25kHz pwm signal

    OCR1BH = 0;
    OCR1BL = 0;  // set this value to the desired fan speed value later in code

    TCNT1H = 0; //set to avoid problems with the output compare -> 16.7.3 Using the Output Compare Unit
    TCNT1L = 0;

    // We have to enable the ports as outputs before PWM will work.
    pinMode(12, OUTPUT);

    /*
    TCNT4 = 0;

    TCCR4A = 0;
    TCCR4B = 0;
    TCCR4C = 0;

    // ------------- 25 kHz PWM and fan PWM pin definition -------------
    TCCR4A = (1 << COM4B1) | (1 << COM4C1) | (1 << WGM41) | (1 << WGM40); // 16.11.1 TCCR1A – Timer/Counter1 Control Register A
    TCCR4B = (1 << WGM43) | (1 << WGM42) | (1 << CS41);  // prescaler = 8
    OCR4AH = 0;
    OCR4AL = OCRCompareValue;  // 79 results in a 25kHz pwm signal

    OCR4BH = 0;
    OCR4BL = 0;

    OCR4CH = 0;
    OCR4CL = 0;

    TCNT4H = 0; //set to avoid problems with the output compare -> 16.7.3 Using the Output Compare Unit
    TCNT4L = 0;

    // We have to enable the ports as outputs before PWM will work.
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    */

    // ------------- FAN speeds -------------
    rpmCalculationTime = millis();
    digitalWrite(fan1RpmInterruptPin, HIGH);
    attachInterrupt(digitalPinToInterrupt(fan1RpmInterruptPin), countRpmSpeedFan1, FALLING);
    digitalWrite(fan2RpmInterruptPin, HIGH);
    attachInterrupt(digitalPinToInterrupt(fan2RpmInterruptPin), countRpmSpeedFan2, FALLING);
    digitalWrite(fan3RpmInterruptPin, HIGH);
    attachInterrupt(digitalPinToInterrupt(fan3RpmInterruptPin), countRpmSpeedFan3, FALLING);
}


void FanController::setFanSpeed()
{
    // all fans share the same pwm output pin. So fan1 can be used as master fan for the speed setting.
    setFanSpeedInternal(fans[1].getSpeed());
}

void FanController::calculateRpmSpeeds()
{
    now = millis();
    if ((rpmCalculationTime + rpmCycleTimeMs) <= now)
    {
        detachInterrupt(fan1RpmInterruptPin);
        detachInterrupt(fan2RpmInterruptPin);
        detachInterrupt(fan3RpmInterruptPin);
        rpmCalculationTime = now;
        fans[1].calculateSpeed();
        fans[2].calculateSpeed();
        fans[3].calculateSpeed();
        attachInterrupt(digitalPinToInterrupt(fan1RpmInterruptPin), countRpmSpeedFan1, FALLING);
        attachInterrupt(digitalPinToInterrupt(fan2RpmInterruptPin), countRpmSpeedFan2, FALLING);
        attachInterrupt(digitalPinToInterrupt(fan3RpmInterruptPin), countRpmSpeedFan3, FALLING);
    }
}

void FanController::saveFanSpeed()
{
    fans[1].saveSpeedToEeprom();
    fans[2].saveSpeedToEeprom();
    fans[3].saveSpeedToEeprom();
}

Fan* FanController::getFan(int id)
{
    if (id >= 1 && id <= 3) {
        return &fans[id];
    }
    return &fans[1];
}

/*
  •	Input Range: The input is expected to be between inMin and inMax (0 to 100 in this case).
  •	Output Range: The output is scaled to be between outMin and outMax (80 to 0).
  •	Mapping Formula: The formula for mapping is:
    mappedValue = ((value - inMin) * (outMax - outMin)) / (inMax - inMin) + outMin;
    for our specific range this can be reduced to value * -outMin / inMax + outMin
    We have to invert the fan speed as the Timer1 settings produce an inverted pwm signal.
*/
unsigned char FanController::mapFanSpeedPercentToPwmValue(int fanSpeedPercent)
{
    double fanSpeedPWM = 0;
    if (fanSpeedPercent >= 100)
    {
        fanSpeedPercent = 100;
    }
    if (fanSpeedPercent <= 0)
    {
        fanSpeedPercent = 0;
    }
    // may not be obvious to everybody: we need the cast to double here as we are dividing by 100. If we stay with int this will result in the fan either being on or off. If we want to have the whole range, we need to be able to calculate with numbers between 0 and 1.
    fanSpeedPWM = static_cast<double>(fanSpeedPercent) / 100 * maxFanSpeed;
    return fanSpeedPWM;
}

// Set the transistor fan speed, where 0 <= fanSpeed <= maxFanSpeed
void FanController::setFanSpeedInternal(unsigned char fanSpeed)
{
    unsigned char mappedFanSpeed = mapFanSpeedPercentToPwmValue(fanSpeed);
    if (OCR1BL != mappedFanSpeed) {

        //Serial.println("Set Fan Speed");
        //Serial.println(fanSpeed);
        //Serial.println(mappedFanSpeed);
        OCR1BH = 0;
        OCR1BL = mappedFanSpeed;
    }
}

void FanController::countRpmSpeedFan1() {
    fans[1].countSpeedImpulse();
}

void FanController::countRpmSpeedFan2() {
    fans[2].countSpeedImpulse();
}

void FanController::countRpmSpeedFan3() {
    fans[3].countSpeedImpulse();
}