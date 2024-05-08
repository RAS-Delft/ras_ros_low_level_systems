#include <Arduino.h>
#include "definitions.h"
#include "shared.h"

long diagnostics_tracker_frequency_main_loop = 0;
unsigned long diagnostics_timestamp = millis();
unsigned long last_reference_received_timestamp = 0;
long diagnostics_tracker_count_telemetry = 0;

/** Monitor system health:
 * - Tracks main loop speed
 * - Reports every period of DIAGNOSTICS_SEND_MESSAGE_PERIOD
 * - (future) Reports battery voltage
 */
void monitor_diagnostics() {
    diagnostics_tracker_frequency_main_loop++;
    if (millis() - diagnostics_timestamp > DIAGNOSTICS_SEND_MESSAGE_PERIOD) {
        // ---- Report ain loop speed
        double main_loop_speed = (diagnostics_tracker_frequency_main_loop*1000) /DIAGNOSTICS_SEND_MESSAGE_PERIOD;
        Serial.print("[Diagnostics] f_main=");
        Serial.print(main_loop_speed);
        Serial.print("hz");
        diagnostics_tracker_frequency_main_loop = 0;
        diagnostics_timestamp = millis();

        double telemFreq = (diagnostics_tracker_count_telemetry*1000) /DIAGNOSTICS_SEND_MESSAGE_PERIOD;
        Serial.print(" f_telem_out=");
        Serial.print(telemFreq);
        Serial.print("hz");
        diagnostics_tracker_count_telemetry = 0;

        Serial.println("");
    }
}
