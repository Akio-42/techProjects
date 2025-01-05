#include <Arduino.h>

void setup() {
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
  OCR1AL = 79;  // This results in a 25kHz pwm signal

  OCR1BH = 0;
  OCR1BL = 0;

  TCNT1H = 0; //set to avoid problems with the output compare -> 16.7.3 Using the Output Compare Unit
  TCNT1L = 0;

  // We have to enable the ports as outputs before PWM will work.
  pinMode(10, OUTPUT);

  Serial.begin(9600);
  Serial.println("Init done.");
}

void loop() {
  Serial.println("------------- 0 % -------------");
  OCR1BH = 0;
  OCR1BL = 0;
  delay(5000);

  Serial.println("------------- 25 % -------------");
  OCR1BH = 0;
  OCR1BL = 20;
  delay(5000);

  Serial.println("------------- 50 % -------------");
  OCR1BH = 0;
  OCR1BL = 40;
  delay(5000);

  Serial.println("------------- 75 % -------------");
  OCR1BH = 0;
  OCR1BL = 60;
  delay(5000);

  Serial.println("------------- 100 % ------------");
  OCR1BH = 0;
  OCR1BL = 78;
  delay(5000);
}

