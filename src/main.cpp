
#include <Arduino.h>
#include <Wire.h>
#include <ST7032_asukiaaa.h>
#include "voltage_read.h"
#include "current_sensor.h"

ST7032_asukiaaa lcd(0x3E);

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  calibrateCurrentSensor();

  // Initialize LCD
  delay(100);
  lcd.begin(16, 2);
  lcd.setContrast(20);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("AC Voltage Meter");

  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(1500);

  lcd.clear();

  Serial.println("AC Source Voltage Measurement");
}

void loop()
{
  // Measure source RMS voltage
  float sourceRMS = readSourceRMS();
  float currentRMS = readCurrentRMS();

  // Display RMS voltage
  lcd.setCursor(0, 0);
  lcd.print("Source RMS:");

  lcd.setCursor(0, 1);
  lcd.print(sourceRMS, 2);
  lcd.print(" V       "); // Clear leftover characters

  // Serial output
  Serial.print("Source RMS Voltage: ");
  Serial.print(sourceRMS, 2);
  Serial.println(" V");

  Serial.print("Current RMS: ");
  Serial.print(currentRMS, 3);
  Serial.println(" A");

  delay(300);
}
