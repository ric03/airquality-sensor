#include <Arduino.h>
#include <Adafruit_SCD30.h>

Adafruit_SCD30 scd30;

#if !defined(ALTITUDE_OFFSET)
#define ALTITUDE_OFFSET 315
#endif

constexpr uint16_t altitude_offset = ALTITUDE_OFFSET; // meters above sea level

#if !defined(TEMPERATURE_OFFSET)
#define TEMPERATURE_OFFSET 400 // 4.00 degree Celsius
#endif

constexpr uint16_t temperature_offset = TEMPERATURE_OFFSET;

void run_sensor_calibration();
void print_sensor_configuration();


void setup()
{
    Serial.begin(115200);

#if WAIT_FOR_SERIAL_CONSOLE == 1
    while (!Serial) delay(100); // will pause until serial console opens
#endif

    // Try to initialize!
    if (!scd30.begin())
    {
        Serial.println("Failed to find SCD30 chip");
        while (true) { delay(1000); }
    }
    Serial.println("SCD30 Found!");
    Serial.println();

#if RUN_SENSOR_CALIBRATION == 1
    run_sensor_calibration();
#endif

    print_sensor_configuration();
}

void loop()
{
    if (scd30.dataReady())
    {
        if (!scd30.read())
        {
            Serial.println("Error reading sensor data");
            return;
        }

        Serial.print("Temperature: ");
        Serial.print(scd30.temperature);
        Serial.println(" degrees C");

        Serial.print("Relative Humidity: ");
        Serial.print(scd30.relative_humidity);
        Serial.println(" %");

        Serial.print("CO2: ");
        Serial.print(scd30.CO2, 3);
        Serial.println(" ppm");
        Serial.println("");
    }

    delay(2000);
}


void run_sensor_calibration()
{
    Serial.println("Calibration started");
    /* Set an altitude offset in meters above sea level.
     * Offset value stored in non-volatile memory of SCD30.
     * Setting an altitude offset will override any pressure offset.
     */
    if (!scd30.setAltitudeOffset(altitude_offset))
    {
        Serial.println("Failed to set altitude offset");
        while (true) { delay(1000); }
    }

    /* Set a temperature offset in hundredths of a degree celcius.
     * Offset value stored in non-volatile memory of SCD30.
     */
    if (!scd30.setTemperatureOffset(temperature_offset))
    {
        Serial.println("Failed to set temperature offset");
        while (true) { delay(1000); }
    }

    /* Enable or disable automatic self calibration (ASC).
     * Parameter stored in non-volatile memory of SCD30.
     * Enabling self calibration will override any previously set
     * forced calibration value.
     * ASC needs continuous operation with at least 1 hour
     * 400ppm CO2 concentration daily.
     */
    if (!scd30.selfCalibrationEnabled(false))
    {
        Serial.println("Failed to enable or disable self calibration");
        while (true) { delay(1000); }
    }

    /* Force the sensor to recalibrate with the given reference value
     * from 400-2000 ppm. Writing a recalibration reference will overwrite
     * any previous self calibration values.
     * Reference value stored in non-volatile memory of SCD30.
     */
    if (!scd30.forceRecalibrationWithReference(400))
    {
        Serial.println("Failed to force recalibration with reference");
        while (true) { delay(1000); }
    }
    Serial.println("Calibration finished");
}

void print_sensor_configuration()
{
    Serial.println("Sensor Configuration");
    Serial.println("====================");

    Serial.print("Measurement interval: ");
    Serial.print(scd30.getMeasurementInterval());
    Serial.println(" seconds");

    Serial.print("Ambient pressure offset: ");
    Serial.print(scd30.getAmbientPressureOffset());
    Serial.println(" mBar");

    Serial.print("Altitude offset: ");
    Serial.print(scd30.getAltitudeOffset());
    Serial.println(" meters");

    Serial.print("Temperature offset: ");
    Serial.print(static_cast<float>(scd30.getTemperatureOffset()) / 100.0);
    Serial.println(" degrees C");

    Serial.print("Forced Recalibration reference: ");
    Serial.print(scd30.getForcedCalibrationReference());
    Serial.println(" ppm");

    if (scd30.selfCalibrationEnabled())
    {
        Serial.println("Self calibration enabled");
    }
    else
    {
        Serial.println("Self calibration disabled");
    }
    Serial.println();
}
