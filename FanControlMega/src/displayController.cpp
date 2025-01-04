#include <displayController.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Encoder.h>
#include <fanController.h>
#include <U8g2lib.h>
#include <config.h>

// LCD Background led and rotary encoder led
Adafruit_NeoPixel DisplayController::strip = Adafruit_NeoPixel(3, NEOPIXEL_PIN, NEO_GRB);

// LCD Display
// clock, data, cs, dc, reset
U8G2_ST7567_JLX12864_1_4W_SW_SPI DisplayController::u8g2_lcd = U8G2_ST7567_JLX12864_1_4W_SW_SPI(U8G2_R0,
    LCD_CLOCK,
    LCD_MOSI,
    LCD_CS,
    LCD_RS,
    LCD_RESET);

// Rotary Encoder from encoder.h
// interrupt pins
Encoder DisplayController::rotaryEncoder = Encoder(ENCODER_PIN1, ENCODER_PIN2);

long DisplayController::rotaryEncoderPosition = 0L;
int DisplayController::currentView = 0;
bool DisplayController::displayOnState = true;
bool DisplayController::lcdClearedState = false;
unsigned long DisplayController::displayShutoffTime = 0L;

void DisplayController::init() {
    // ------------- Neopixel -------------
    activateLeds();

    // ------------- Display -------------
    u8g2_lcd.begin();
    u8g2_lcd.setContrast(180);  // This is extremely important

    // ------------- Rotary Encoder Button -------------
    attachInterrupt(digitalPinToInterrupt(2), encoder_pressed, FALLING);
    pinMode(2, INPUT_PULLUP);

    // Init display shutoff timer
    displayShutoffTime = millis();
}

/***********************************
------------- UI Methods -----------
************************************/
// render the ui to the display
void DisplayController::drawContentOnLcd() {
    if (displayOnState) {

        Fan* fan1 = FanController::getFan(1);
        Fan* fan2 = FanController::getFan(2);
        Fan* fan3 = FanController::getFan(3);

        u8g2_lcd.clearBuffer();
        u8g2_lcd.firstPage();
        do {
            u8g2_lcd.setFont(u8g2_font_6x12_mf);
            u8g2_lcd.setFontMode(0);
            u8g2_lcd.setDrawColor(1);

            if (currentView == 0) {
                // draw main screen
                drawGridLayout();
                drawFanInfo(fan1);
                drawFanInfo(fan2);
                drawFanInfo(fan3);
            }
            else {
                // draw detail screen
                drawBoxLayout();
                drawFanSettingsView(fan1);
            }
        } while (u8g2_lcd.nextPage());
    }
    else if (!lcdClearedState) {
        u8g2_lcd.clear();
        lcdClearedState = true;
    }
}

void DisplayController::drawGridLayout() {
    // top border
    u8g2_lcd.drawHLine(0, 0, 128);
    // bottom border
    u8g2_lcd.drawHLine(0, 63, 128);
    // left border
    u8g2_lcd.drawVLine(0, 1, 62);
    // right border
    u8g2_lcd.drawVLine(127, 1, 62);
    // left divider
    u8g2_lcd.drawVLine(42, 1, 62);
    // right divier
    u8g2_lcd.drawVLine(84, 1, 62);
}

void DisplayController::drawBoxLayout() {
    // top border
    u8g2_lcd.drawHLine(0, 0, 128);
    // bottom border
    u8g2_lcd.drawHLine(0, 63, 128);
    // left border
    u8g2_lcd.drawVLine(0, 1, 62);
    // right border
    u8g2_lcd.drawVLine(127, 1, 62);
}

void DisplayController::drawFanInfo(Fan* fan) {
    int column = 0;
    switch (fan->getId())
    {
    case 1:
        column = 5;
        break;
    case 2:
        column = 47;
        break;
    case 3:
        column = 89;
        break;

    default:
        break;
    }

    u8g2_lcd.drawStr(column, 12, fan->getDisplayName().c_str());
    u8g2_lcd.drawStr(column, 25, "Speed");
    u8g2_lcd.drawStr(column, 35, fan->getDisplaySpeed().c_str());
    u8g2_lcd.drawStr(column, 50, "RPM");
    u8g2_lcd.drawStr(column, 60, fan->getDisplayRPM().c_str());
}

void DisplayController::drawFanSettingsView(Fan* fan) {
    u8g2_lcd.drawStr(5, 12, "All Fans");
    u8g2_lcd.drawStr(5, 25, "Speed");
    u8g2_lcd.drawStr(5, 35, fan->getDisplaySpeed().c_str());
}

void DisplayController::activateLeds() {
    // ------------- Neopixel -------------
    // The color order is Green, Red, Blue.
    strip.begin();
    strip.setBrightness(32);
    strip.setPixelColor(0, strip.Color(255, 255, 255)); // display background
    strip.setPixelColor(1, strip.Color(255, 0, 0)); // left encoder wheel led
    strip.setPixelColor(2, strip.Color(0, 0, 255)); // right encoder wheel led
    strip.show();
}

/***********************************
------------- Input Handling -------------
************************************/
void DisplayController::handleInputs() {
    int encoderState = getEncoderMovedState();
    if (encoderState != 0) {
        activateDisplayOnEncoderMove();
        delay(150); // small delay so turning the encoder feels more natural
        if (currentView == 1) {
            setFanSpeedSetting(encoderState);
        }
    }
}

int DisplayController::getEncoderMovedState() {
    long newRotaryEncoderPosition = rotaryEncoder.read();
    if (newRotaryEncoderPosition > rotaryEncoderPosition) {
        rotaryEncoderPosition = newRotaryEncoderPosition;
        // rotary encoder was turned counter clock wise. Return -1.
        return -1;
    }
    else if (newRotaryEncoderPosition < rotaryEncoderPosition) {
        rotaryEncoderPosition = newRotaryEncoderPosition;
        // rotary encoder was turned clock wise. Return 1.
        return 1;
    }
    // rotary encoder was not turned. Return 0.
    return 0;
}

// reset the time on each encoder move but only switch on stuff and change state if the display was really off
void DisplayController::activateDisplayOnEncoderMove() {
    if (!displayOnState) {
        activateLeds();
        displayOnState = true;
        lcdClearedState = false;
    }
    displayShutoffTime = millis();
}

void DisplayController::shutoffDisplayIfNotInUse() {
    if ((displayShutoffTime + 60000) <= millis() && displayOnState)
    {
        displayOnState = false;
        displayShutoffTime = 0;
        strip.setBrightness(0);
        strip.show();
    }
}

void DisplayController::encoder_pressed() {
    if (currentView == 1) {
        // switch to main view and save the fan speed.
        currentView = 0;
        FanController::saveFanSpeed();
    }
    else {
        // switch to detail view
        currentView = 1;
    }
    activateDisplayOnEncoderMove();
    delay(250); // avoid ghost pushes
}

void DisplayController::setFanSpeedSetting(int encoderMoved) {
    Fan* fan1 = FanController::getFan(1);
    Fan* fan2 = FanController::getFan(2);
    Fan* fan3 = FanController::getFan(3);
    if (encoderMoved == 1 && fan1->getSpeed() <= 90) {
        fan1->setSpeed(fan1->getSpeed() + 10);
        // all fans run the same speed for now as they share the pwm output. Setting the fan speed on each object is only done for display purposes
        fan2->setSpeed(fan1->getSpeed());
        fan3->setSpeed(fan1->getSpeed());
    }
    else if (encoderMoved == -1 && fan1->getSpeed() >= 10) {
        fan1->setSpeed(fan1->getSpeed() - 10);
        // all fans run the same speed for now as they share the pwm output. Setting the fan speed on each object is only done for display purposes
        fan2->setSpeed(fan1->getSpeed());
        fan3->setSpeed(fan1->getSpeed());
    }
}