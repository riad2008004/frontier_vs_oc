
#include <Arduino.h>
#include <math.h>
#include "current_sensor.h"

static const uint8_t CURRENT_PIN = A0;

static const float ADC_REF = 5.0f;
static const float ADC_MAX = 1023.0f;

// ACS712ELCTR-30A-T
static const float SENSITIVITY = 0.066f; // V/A

static float zeroOffsetADC = 512.0f;
static bool isCalibrated = false;

void calibrateCurrentSensor()
{
    const unsigned int NUM_SAMPLES = 2000;
    double adcSum = 0.0;

    // Ensure the measured current is zero during calibration.
    delay(200);

    for (unsigned int i = 0; i < NUM_SAMPLES; i++)
    {
        adcSum += analogRead(CURRENT_PIN);
        delayMicroseconds(250);
    }

    zeroOffsetADC = adcSum / NUM_SAMPLES;
    isCalibrated = true;

    Serial.print("Zero offset ADC: ");
    Serial.println(zeroOffsetADC, 2);

    Serial.print("Zero offset voltage: ");
    Serial.print(zeroOffsetADC * ADC_REF / ADC_MAX, 3);
    Serial.println(" V");
}

float readCurrentRMS()
{
    if (!isCalibrated)
        return 0.0f;

    double sumSquares = 0.0;
    unsigned long samples = 0;

    // 200 ms captures approximately 10 cycles at 50 Hz.
    unsigned long startTime = millis();

    while (millis() - startTime < 200)
    {
        int adcValue = analogRead(CURRENT_PIN);

        // Subtract offset in ADC counts before conversion.
        float deltaADC = adcValue - zeroOffsetADC;
        float deltaVoltage = deltaADC * ADC_REF / ADC_MAX;

        float current = deltaVoltage / SENSITIVITY;

        sumSquares += (double)current * current;
        samples++;
    }

    if (samples == 0)
        return 0.0f;

    return sqrt(sumSquares / samples);
}

float getCurrentOffsetVoltage()
{
    return zeroOffsetADC * ADC_REF / ADC_MAX;
}
