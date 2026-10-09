
#include <Arduino.h>
#include <ST7032_asukiaaa.h>
#include "menu.h"
#include "button.h"
#include "menu.h"

static bool opened = false;
static bool holdTiming = false;
static unsigned long holdStart = 0;
static unsigned long lastButtonAction = 0;

static const unsigned long ENTER_HOLD_MS = 3000;
static const unsigned long DEBOUNCE_MS = 180;

enum SelectedItem
{
    SELECT_VOLTAGE,
    SELECT_CURRENT
};

static SelectedItem selected = SELECT_VOLTAGE;

static const float MIN_VOLTAGE = 180.0;
static const float MAX_VOLTAGE_LIMIT = 260.0;
static const float MIN_CURRENT = 1.0;
static const float MAX_CURRENT_LIMIT = 30.0;

static void drawMenu(ST7032_asukiaaa &lcd,
                     float voltage,
                     float current)
{
    lcd.clear();

    if (selected == SELECT_VOLTAGE)
    {
        lcd.setCursor(0, 0);
        lcd.print(">Voltage limit");

        lcd.setCursor(0, 1);
        lcd.print(voltage, 0);
        lcd.print("V L-/R+ SET");
    }
    else
    {
        lcd.setCursor(0, 0);
        lcd.print(">Current limit");

        lcd.setCursor(0, 1);
        lcd.print(current, 1);
        lcd.print("A L-/R+ SET");
    }
}

static bool menuAnyButtonPressed()
{
    return isUpPressed() ||
           isRightPressed() ||
           isSetPressed() ||
           isLeftPressed() ||
           isDownPressed();
}

void menuBegin(ST7032_asukiaaa &lcd,
               float initialVoltage,
               float initialCurrent)
{
    opened = false;
    holdTiming = false;
    selected = SELECT_VOLTAGE;

    menuShowStartup(lcd, initialVoltage, initialCurrent);
}

void menuShowStartup(ST7032_asukiaaa &lcd,
                     float maxVoltage,
                     float maxCurrent)
{
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("V:");
    lcd.print(maxVoltage, 0);
    lcd.print(" I:");
    lcd.print(maxCurrent, 1);

    lcd.setCursor(0, 1);
    lcd.print("Hold btn 3s:SET");
}

bool menuIsOpen()
{
    return opened;
}

MenuResult menuHandle(
    ST7032_asukiaaa &lcd,
    float &maxVoltage,
    float &maxCurrent)
{
    bool pressed = menuAnyButtonPressed();

    if (!opened)
    {
        if (pressed)
        {
            if (!holdTiming)
            {
                holdTiming = true;
                holdStart = millis();
            }

            if (millis() - holdStart >= ENTER_HOLD_MS)
            {
                opened = true;
                holdTiming = false;
                selected = SELECT_VOLTAGE;

                drawMenu(lcd, maxVoltage, maxCurrent);
                return MENU_ENTERED;
            }
        }
        else
        {
            holdTiming = false;
        }

        return MENU_IDLE;
    }

    // Avoid repeating an action too rapidly.
    if (!pressed ||
        millis() - lastButtonAction < DEBOUNCE_MS)
    {
        return MENU_IDLE;
    }

    if (isUpPressed())
    {
        selected = SELECT_VOLTAGE;
    }
    else if (isDownPressed())
    {
        selected = SELECT_CURRENT;
    }
    else if (isLeftPressed())
    {
        if (selected == SELECT_VOLTAGE)
        {
            maxVoltage -= 1.0;
            if (maxVoltage < MIN_VOLTAGE)
                maxVoltage = MIN_VOLTAGE;
        }
        else
        {
            maxCurrent -= 0.5;
            if (maxCurrent < MIN_CURRENT)
                maxCurrent = MIN_CURRENT;
        }
    }
    else if (isRightPressed())
    {
        if (selected == SELECT_VOLTAGE)
        {
            maxVoltage += 1.0;
            if (maxVoltage > MAX_VOLTAGE_LIMIT)
                maxVoltage = MAX_VOLTAGE_LIMIT;
        }
        else
        {
            maxCurrent += 0.5;
            if (maxCurrent > MAX_CURRENT_LIMIT)
                maxCurrent = MAX_CURRENT_LIMIT;
        }
    }
    else if (isSetPressed())
    {
        opened = false;
        holdTiming = false;
        lastButtonAction = millis();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Settings saved");
        lcd.setCursor(0, 1);
        lcd.print("Returning...");
        return MENU_SAVED;
    }

    lastButtonAction = millis();
    drawMenu(lcd, maxVoltage, maxCurrent);

    return MENU_IDLE;
}
