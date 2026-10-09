
#include <Arduino.h>
#include "button.h"

// Button pin assignments
static const uint8_t BUTTON_UP = 6;
static const uint8_t BUTTON_RIGHT = 5;
static const uint8_t BUTTON_SET = 4;
static const uint8_t BUTTON_LEFT = 3;
static const uint8_t BUTTON_DOWN = 2;

void initButtons()
{
    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_RIGHT, INPUT_PULLUP);
    pinMode(BUTTON_SET, INPUT_PULLUP);
    pinMode(BUTTON_LEFT, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);
}

// LOW means pressed because INPUT_PULLUP is enabled
bool isUpPressed()
{
    return digitalRead(BUTTON_UP) == LOW;
}

bool isRightPressed()
{
    return digitalRead(BUTTON_RIGHT) == LOW;
}

bool isSetPressed()
{
    return digitalRead(BUTTON_SET) == LOW;
}

bool isLeftPressed()
{
    return digitalRead(BUTTON_LEFT) == LOW;
}

bool isDownPressed()
{
    return digitalRead(BUTTON_DOWN) == LOW;
}
