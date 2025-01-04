#include <Arduino.h>
#include <fanController.h>
#include <displayController.h>

void setup() {
  // ------------- Init Mini 12864 Display -------------
  DisplayController::init();

  // ------------- FanController -------------
  FanController::init();

  //Serial.begin(9600);
  //Serial.println("Init finished.");
}

void loop() {
  DisplayController::handleInputs();
  DisplayController::drawContentOnLcd();
  DisplayController::shutoffDisplayIfNotInUse();
  delay(10);

  FanController::setFanSpeed();
  FanController::calculateRpmSpeeds();

}

