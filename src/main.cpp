
#include <Arduino.h>
#include <Wire.h>
#include <ST7032_asukiaaa.h>

#include "voltage_read.h"
#include "current_sensor.h"
#include "button.h"
#include "menu.h"

// LCD
ST7032_asukiaaa lcd(0x3E);

// Pin configuration
const uint8_t RELAY_PIN = 7;

// Protection thresholds
float MAX_VOLTAGE = 220.0;
float MAX_CURRENT = 16.0;

// Set according to your relay module:
// LOW = relay energized, load connected (common active-LOW modules)
// HIGH = relay de-energized, load disconnected
const uint8_t RELAY_ON = LOW;
const uint8_t RELAY_OFF = HIGH;

// Fault state
enum FaultType
{
  NO_FAULT,
  OVER_VOLTAGE,
  OVER_CURRENT,
  OVER_VOLTAGE_AND_CURRENT
};

FaultType fault = NO_FAULT;

// Measurement variables
float voltageRMS = 0.0;
float currentRMS = 0.0;

// Timing
unsigned long lastMeasurement = 0;
const unsigned long MEASUREMENT_INTERVAL = 250;

unsigned long resetHoldStart = 0;
bool resetTiming = false;

// --------------------------------------------------
// Button functions
// --------------------------------------------------

bool anyButtonPressed()
{
  return isUpPressed() ||
         isRightPressed() ||
         isSetPressed() ||
         isLeftPressed() ||
         isDownPressed();
}

// --------------------------------------------------
// Relay control
// --------------------------------------------------

void disconnectLoad()
{
  digitalWrite(RELAY_PIN, RELAY_OFF);
}

void connectLoad()
{
  digitalWrite(RELAY_PIN, RELAY_ON);
}

// --------------------------------------------------
// LCD functions
// --------------------------------------------------

void showNormalScreen()
{
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltageRMS, 1);
  lcd.print("V I:");
  lcd.print(currentRMS, 1);
  lcd.print("A ");

  lcd.setCursor(0, 1);
  lcd.print("STATUS: NORMAL ");
}

void showFaultScreen()
{
  lcd.clear();

  lcd.setCursor(0, 0);

  if (fault == OVER_VOLTAGE)
  {
    lcd.print("FAULT: OVER-V");
  }
  else if (fault == OVER_CURRENT)
  {
    lcd.print("FAULT: OVER-I");
  }
  else if (fault == OVER_VOLTAGE_AND_CURRENT)
  {
    lcd.print("FAULT: OVER V/I");
  }

  lcd.setCursor(0, 1);
  lcd.print("Hold btn 5s reset");
}

// --------------------------------------------------
// Fault detection
// --------------------------------------------------

void checkFaults()
{
  bool overVoltage = voltageRMS > MAX_VOLTAGE;
  bool overCurrent = currentRMS > MAX_CURRENT;

  if (overVoltage && overCurrent)
  {
    fault = OVER_VOLTAGE_AND_CURRENT;
  }
  else if (overVoltage)
  {
    fault = OVER_VOLTAGE;
  }
  else if (overCurrent)
  {
    fault = OVER_CURRENT;
  }

  if (fault != NO_FAULT)
  {
    disconnectLoad();
    showFaultScreen();
  }
}

// --------------------------------------------------
// Reset handling
// --------------------------------------------------

void handleReset()
{
  if (fault == NO_FAULT)
  {
    resetTiming = false;
    return;
  }

  if (anyButtonPressed())
  {
    if (!resetTiming)
    {
      resetTiming = true;
      resetHoldStart = millis();
    }

    // Hold any button for 5 seconds
    if (millis() - resetHoldStart >= 5000)
    {

      // Only reset when BOTH conditions are safe
      if (voltageRMS <= MAX_VOLTAGE &&
          currentRMS <= MAX_CURRENT)
      {

        fault = NO_FAULT;
        resetTiming = false;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Fault cleared");
        lcd.setCursor(0, 1);
        lcd.print("Load reconnect");
        delay(1000);

        connectLoad();
        lcd.clear();
      }
      else
      {
        resetTiming = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Still unsafe!");
        lcd.setCursor(0, 1);
        lcd.print("Load disconnected");
        delay(1000);
        showFaultScreen();
      }
    }
  }
  else
  {
    // Button released before 5 seconds
    resetTiming = false;
  }
}

// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  // Keep the load disconnected during initialization
  pinMode(RELAY_PIN, OUTPUT);
  disconnectLoad();
  initButtons();

  delay(100);
  lcd.begin(16, 2);
  lcd.setContrast(20);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Voltage Protect");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(1500);

  // Calibrate current sensor with zero current.
  // Keep the load disconnected during this calibration.
  calibrateCurrentSensor();

  lcd.clear();

  menuBegin(lcd, MAX_VOLTAGE, MAX_CURRENT);

  connectLoad();
}

// --------------------------------------------------
// Main loop
// --------------------------------------------------

void loop()
{
  unsigned long now = millis();

  if (now - lastMeasurement >= MEASUREMENT_INTERVAL)
  {
    lastMeasurement = now;

    voltageRMS = readSourceRMS();
    currentRMS = readCurrentRMS();

    MenuResult result = MENU_IDLE;
    if (fault == NO_FAULT)
    {
      result = menuHandle(lcd, MAX_VOLTAGE, MAX_CURRENT);
    }

    if (fault == NO_FAULT && menuIsOpen())
    {
      disconnectLoad();
      return;
    }

    if (result == MENU_SAVED)
    {
      if (voltageRMS > MAX_VOLTAGE ||
          currentRMS > MAX_CURRENT)
      {
        checkFaults();
      }
      else
      {
        fault = NO_FAULT;
        connectLoad();
        lcd.clear();
      }
      return;
    }

    Serial.print("Voltage: ");
    Serial.print(voltageRMS, 2);
    Serial.print(" V | Current: ");
    Serial.print(currentRMS, 2);
    Serial.println(" A");

    if (fault == NO_FAULT)
    {
      checkFaults();

      if (fault == NO_FAULT)
      {
        showNormalScreen();
      }
    }
    else
    {
      // Keep the relay disconnected until reset
      disconnectLoad();
    }
  }

  handleReset();
}
