#include "definitions.h"
#include "shared.h"
#include <Arduino.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <EEPROM.h>


// BNO055 Heading sensor parameters
int eeAddress = 0;
long bnoID;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

/**
 * A little proxy function for getting the IMU vectors
 * @param vector_type The vector that you want
 */
imu::Vector<3> getIMUVector(Adafruit_BNO055::adafruit_vector_type_t vector_type) {
    return bno.getVector(vector_type);
}

/**
 * Attempts to connect to the BNO055 sensor and initiate it.
 * - Will check existence of device
 * - Will look for earlier calibration data in EEPROM and load it if existing
 * - Otherwise: will force user to calibrate the sensor and save it,
 */
void initialize_IMU() {

    /* Initialise the sensor */
    if (!bno.begin())
    {
        /* There was a problem detecting the BNO055 ... check your connections */
        Serial.print("[Warning] No BNO055 detected ... Check your wiring or I2C ADDR!");
        IMU_enabled = false;
    } else {
        Serial.println("[Info] BNO055 device detected");
    }
    if (IMU_enabled) {
        EEPROM.get(eeAddress, bnoID);
        adafruit_bno055_offsets_t calibrationData;
        sensor_t sensor;
        bool foundCalib = false;

        /*
           Look for the sensor's unique ID at the beginning oF EEPROM.
           This isn't foolproof, but it's better than nothing.
        */
        bno.getSensor(&sensor);
        if (bnoID != sensor.sensor_id)
        {
            Serial.println("[Info] No Calibration Data for this sensor exists in EEPROM");
        }
        else
        {
            Serial.println("[Info] Found Calibration for this sensor in EEPROM, which will be used");
            eeAddress += sizeof(long);
            EEPROM.get(eeAddress, calibrationData);
            bno.setSensorOffsets(calibrationData);
            foundCalib = true;
        }

        delay(100);

        /* Crystal must be configured AFTER loading calibration data into BNO055. */
        bno.setExtCrystalUse(true);

        sensors_event_t event;
        bno.getEvent(&event);

        /*
           Run calibration sequence if no EEPROM save could be found.
        */
        bool requireCalib = false; // Disable the need of calibration, as it is currently (10 okt 2022) not used, and only annoying. Might use the sensor's internal state estimation later at some point, so leaving it here. [BB okt 2022]
        if (!foundCalib && requireCalib) {
            Serial.println("[Calibrating BNO055] Please Calibrate BNO055: ");
            while (!bno.isFullyCalibrated())
            {
                bno.getEvent(&event);

                Serial.print("[Calibrating BNO055] X: ");
                Serial.print(event.orientation.x, 4);
                Serial.print("\tY: ");
                Serial.print(event.orientation.y, 4);
                Serial.print("\tZ: ");
                Serial.print(event.orientation.z, 4);

                /* Display calibration status for each sensor. */
                uint8_t system, gyro, accel, mag = 0;
                bno.getCalibration(&system, &gyro, &accel, &mag);
                Serial.print("CAL: Sys=");
                Serial.print(system, DEC);
                Serial.print(" Gyro=");
                Serial.print(gyro, DEC);
                Serial.print(" Accel=");
                Serial.print(accel, DEC);
                Serial.print(" Mag=");
                Serial.println(mag, DEC);

                /* Wait the specified delay before requesting new data */
                delay(100);
            }
            saveCalibEEPROM();
        }
        delay(200);
    }
}

/** Store the current calibration data in arduino's EEPROM for future use */
void saveCalibEEPROM() {
    Serial.print("[Info] Storing IMU calibration data to EEPROM. ");
    sensor_t sensor;
    adafruit_bno055_offsets_t newCalib;
    bno.getSensorOffsets(newCalib);
    eeAddress = 0;
    bno.getSensor(&sensor);
    bnoID = sensor.sensor_id;
    EEPROM.put(eeAddress, bnoID);
    eeAddress += sizeof(long);
    EEPROM.put(eeAddress, newCalib);
}
