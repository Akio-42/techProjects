# Test project for a 25 kHz pwm fan control with an Arduino uno.
This project sets pwm pin 10 to a 25 kHz signal by manipulating the timer1 registers.
Normally the Arduino has a frequency of 490 Hz for its PWM pins. To change that to 25 kHz the follwing adaptions have to be made:
* Clear the three timer registers of timer 1 - TCCR1A, TCCR1B, TCCR1C.
* Set the PWM mode of timer 1 to fast pwm with OCR1A as TOP of timer 1 -  WGM11, WGM10, WGM13, WGM12
* Set prescaler of timer 1 to 8 -  CS11
* Set OCR1B as compare match (non-inverting) for the Compare Output Mode of timer 1 in fast PWM - COM1B1 
* Set pinMode of pin 12 to output
* Configure OCR1AH/OCR1AL value
* Configure OCR1BH/OCR1BL value

## Explanation
In fast pwm mode the timer counts from 0 to the value set to OCR1AL. By setting OCR1AL to a specific value we can manipulate how long the timer is couting up. With this we can manipulate the frequency.
To get into the right target range for 25 kHz we need to set the prescaler to 8.
By setting the Compare Output Mode of timer 1 to non-inverting mode it compares against the value in OCR1BL. When the timer hits the value set to OCR1BL it switches pin 12 on/off and then counts up until it reaches the TOP value defined in OCR1AL. This allows us to set the duty cycle.
To my understanding ping digital pin 9 cannot be used any more for pwm output with this configuration.

## Test program
The test program sets the pwm frequency to 25 kHz (OCR1AL = 79). Then it remains for 5 seconds at 0% and goes up in stages to 100% duty cycle.
Measured PWM frequencies:
------------- 0 % --------------
![measurement 0 percent duty cycle](./resources/Measurement01.png)
------------- 25 % -------------
![measurement 25 percent duty cycle](./resources/Measurement02.png)
------------- 50 % -------------
![measurement 50 percent duty cycle](./resources/Measurement03.png)
------------- 75 % -------------
![measurement 75 percent duty cycle](./resources/Measurement04.png)
------------- 100 % ------------
![measurement 100 percent duty cycle](./resources/Measurement05.png)

## Additional Resources:
* https://wolles-elektronikkiste.de/timer-und-pwm-teil-2-16-bit-timer1 Article how Arduino timers can be manipulated
* http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf ATmega data sheet

## Please note:
This project and all code I wrote are provided as-is and are intended for educational purposes only. While this information is meant to guide and inspire other makers, there is no guarantee that the code is error-free or suitable for every situation. I am not responsible for any damage or issues that arise from simply copying and pasting the provided code without proper understanding or modification. Always review and test code thoroughly before using it in your own projects.