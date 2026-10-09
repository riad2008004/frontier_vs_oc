#include <Arduino.h>
#include <math.h>

const int voltagePin = A1;

const float ADC_REF = 5.0;
const float ADC_MAX = 1023.0;

const float SENSOR_MID = 3.35;
const float SENSOR_PEAK = 0.95;
const float SOURCE_PEAK = 312.0;

float readSourceRMS()
{
    double sumSquares = 0.0;
    unsigned long samples = 0;

    unsigned long startTime = millis();

    // Sample for 200 ms (10 cycles at 50 Hz)
    while (millis() - startTime < 200)
    {
        int adcValue = analogRead(voltagePin);

        float sensorVoltage =
            adcValue * ADC_REF / ADC_MAX;

        float sourceVoltage =
            (sensorVoltage - SENSOR_MID) * SOURCE_PEAK / SENSOR_PEAK;

        sumSquares +=
            (double)sourceVoltage * sourceVoltage;

        samples++;
    }

    if (samples == 0)
    {
        return 0.0;
    }

    return sqrt(sumSquares / samples);
}
