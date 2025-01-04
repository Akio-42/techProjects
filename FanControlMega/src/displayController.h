#ifndef DISPLAYCONTROLLER_H
#define DISPLAYCONTROLLER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Encoder.h>
#include <fan.h>
#include <fanController.h>
#include <U8g2lib.h>

class DisplayController {
private:
    // private constructor
    DisplayController() {};

    // the neo pixels of the display (button and background light)
    static Adafruit_NeoPixel strip;
    // the rotary encoder of the display
    static Encoder rotaryEncoder;
    // the lcd display
    static U8G2_ST7567_JLX12864_1_4W_SW_SPI u8g2_lcd;

    // this variable keeps track of the rotary encoder position. It is mainly used to determine in which direction the knob was turned.
    static long rotaryEncoderPosition;
    // this variable keeps track which screen is shown (main screen or detail screen)
    static int currentView;
    // keeps track if the display background leds are on/off
    static bool displayOnState;
    // keeps track if the lcd was cleard for the power off state
    static bool lcdClearedState;

    // the time interval after which the display should turn off
    static unsigned long displayShutoffTime;

    // draw method for the three column grid layout of the main screen
    static void drawGridLayout();

    // draw method for the box layout of the detail screen
    static void drawBoxLayout();

    // method to draw the fan details in the columns on the main screen
    static void drawFanInfo(Fan* fan);

    // method to draw the contents of the detail screen
    static void drawFanSettingsView(Fan* fan);

    // encoder methods
    // check if the encoder moved and return the information if and if yes in which direction
    static int getEncoderMovedState();

    // interrupt triggered method when the encoder wheel button was pressed
    static void encoder_pressed();

    // state methods
    // set the fan speed setting on the respective fan object when the encoder wheel was moved when the detail screen is displayed
    static void setFanSpeedSetting(int encoderMoved);

    static void activateDisplayOnEncoderMove();

    // switch on button an background leds
    static void activateLeds();

public:
    // code called during setup
    static void init();

    // main method to draw stuff on the lcd
    static void drawContentOnLcd();

    // handle inputs
    static void handleInputs();

    // shutoff display when it is not used e.g. no one is looking at it
    static void shutoffDisplayIfNotInUse();
};

#endif // DISPLAYCONTROLLER_H