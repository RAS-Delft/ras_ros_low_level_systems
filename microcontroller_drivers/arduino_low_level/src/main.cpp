// Main libraries
#include <Arduino.h>
#include <Servo.h>
#include <PID_v1.h>
#include "definitions.h"
#include "shared.h"

uint32_t last_serial_message_telemetry_sent = millis();

bool IMU_enabled = true;

void setup() {
    // Initialize Serial Communication
    Serial.begin(115200);
    Serial.println("[Info] (Re)started");

    // Initialize BNO055 9 axis sensor
    if (IMU_enabled) {
        initialize_IMU();
    }

    delay(100);

    // Configure PID
    normalized_output_BOW = 0;

    // Configure interrupts for calculating RPM
    attachInterrupt(digitalPinToInterrupt(interruptPin_P), rpm_interrupt_P, FALLING);
    attachInterrupt(digitalPinToInterrupt(interruptPin_SB), rpm_interrupt_SB, FALLING);

    Serial.println("[Info] End of initialization");
}

enum WatchDogState {
    NORMAL,
    WAITING
};

enum WatchDogState watchDogState_ref_timeout = WAITING;

void loop() {
    // -------------------------------------- Update state feedback that is used in the controller
    update_sensor_values();

    // -------------------------------------- Reference acquisition timeout
    if (millis() - last_reference_received_timestamp > LAST_REFERENCE_RECEIVED_WATCHDOG) {
        // Turn off everything if last instructions were too long ago
        if (watchDogState_ref_timeout == NORMAL) {
            // Turn of engines
            PID_P.setReference(0);
            PID_SB.setReference(0);
            normalized_output_BOW = 0;

            // Set thrusters to default (zero) angle
            reference_Angle_P = 0;
            reference_Angle_SB = 0;
            enable_motors(false);
            Serial.print("[Info] loop() Reference watchdog timeout after "); Serial.print(LAST_REFERENCE_RECEIVED_WATCHDOG); Serial.println(" ms");
            // Set watchdog timeout variable
            watchDogState_ref_timeout = WAITING;
        }
    } else {
        // If we are waiting for a message to start again, and we got one.
        if (watchDogState_ref_timeout == WAITING) {
            enable_motors(true);
            watchDogState_ref_timeout = NORMAL;
        }
    }

    // -------------------------------------- Compute the PIDs
    run_control();

    // -------------------------------------- Send control signals to DC motors
    send_motor_controls();

    // -------------------------------------- Send telemetry
    if (millis() - last_serial_message_telemetry_sent > TELEMETRY_SEND_MESSAGE_PERIOD) {
        serial_send_telemetry();
        last_serial_message_telemetry_sent = millis();
    }

    // -------------------------------------- Track system health and occasionally report
    monitor_diagnostics();
}
