
#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <ST7032_asukiaaa.h>

enum MenuResult
{
    MENU_IDLE,
    MENU_ENTERED,
    MENU_SAVED
};

void menuBegin(ST7032_asukiaaa &lcd,
               float initialVoltage,
               float initialCurrent);

MenuResult menuHandle(
    ST7032_asukiaaa &lcd,
    float &maxVoltage,
    float &maxCurrent);

void menuShowStartup(ST7032_asukiaaa &lcd,
                     float maxVoltage,
                     float maxCurrent);

bool menuIsOpen();

#endif
