//
// Created by Casper Cromjongh on 23/02/2022.
// Updated by Bart Boogmans june 2022
//

#ifndef ARDUINO_PRIMARY_DEFINITIONS_H
#define ARDUINO_PRIMARY_DEFINITIONS_H

/* Assign Arduino Pins -----------------------------------------------------------------------------------*/
const int pin_AZI_P = 4;
const int pin_AZI_SB = 5;

const int interruptPin_P = 2;
const int interruptPin_SB = 3;

const int pin_ESC_P = 7;
const int pin_ESC_SB = 8;
const int pin_ESC_bow = 6;

const int resetPin = 12;

/* Assign PID variables ----------------------------------------------------------------------------------*/
// The input of this controller is rpm error, typically in range of -3000 to +3000
// The output of the controller is in 8 bit integer range (0-255) where 0 means engine standstill and 255 is maximum power.

const float Kp_P = 0.02;    // unit: 8-bit-int/RPM
const float Ki_P = 0.12;     // unit: 8-bit-int/(RPM*second)
const float Kd_P = 0.00;       // unit: (8-bit-int*second)/RPM
const float Kp_SB = 0.02;
const float Ki_SB = 0.12;
const float Kd_SB = 0.00;

/* Set time constants ----------------------------------------------------------------------------------*/
const long TELEMETRY_SEND_MESSAGE_PERIOD = 40;         // Retreives and sends telemetry in each of this period [ms]
const long LAST_REFERENCE_RECEIVED_WATCHDOG = 4000;     // Resets actuator after this amount [ms]
const long DIAGNOSTICS_SEND_MESSAGE_PERIOD = 5000;      // Sends diagnostics messages in this period [ms]
const long ENCODER_RPS_TIMEOUT = 60;

/* Set actuator limits ----------------------------------------------------------------------------------*/

const int MIN_RPM_AFT_THRUSTER = 480;                   // Minimum threshold aft motor speed before actually activating it.  [rpm]
const float MIN_NORM_PWR_BOW_THRUSTER = 0.03;                   // Minimum threshold bow thruster before actually activating it.  [Normalized 0:1.0]
const float GEARRATIO_SERVO_AZIMUTH = 0.6666;

#endif //ARDUINO_PRIMARY_DEFINITIONS_H
