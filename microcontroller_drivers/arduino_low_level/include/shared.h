//
// Created by Casper on 6-7-22.
//

#ifndef ARDUINO_PRIMARY_SHARED_H
#define ARDUINO_PRIMARY_SHARED_H

#include <Arduino.h>
#include <Servo.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "PIDMotor.h"

/*** General Settings ***/
extern bool IMU_enabled; // False disables acquiring telemetry from inertial measurement unit.

// System health monitoring
extern unsigned long last_reference_received_timestamp;
extern long diagnostics_tracker_count_telemetry;

// DC motor (PID)control variables
extern PIDMotor PID_P, PID_SB;
// Bow thruster (feed-forward) control variables
extern double normalized_output_BOW;

/*** Assign Azipod start positions ***/
extern int zeroPoint_AZI_P;  // forward configuration: 82  | reverse configuration: 124
extern int zeroPoint_AZI_SB; // forward configuration: 95 | reverse configuration: 57
extern int reference_Angle_P;
extern int reference_Angle_SB;

/*** Motor control variables ***/
// Units: servo out signal is a number ranging from approximately 1200 (left) to 1500 (mid) to 1800 (right). Desired actuator angle is mapped onto this range before being passed to the servo.write function.
extern int motor_ESC_SignalOut_Bow;
extern int motor_ESC_SignalOut_P;
extern int motor_ESC_SignalOut_SB;

void initialize_IMU();
imu::Vector<3> getIMUVector(Adafruit_BNO055::adafruit_vector_type_t vector_type);
void saveCalibEEPROM();
void monitor_diagnostics();
int find_num_delimiters(String str, char delimiter);

void rpm_interrupt_P();
void rpm_interrupt_SB();
void serial_send_telemetry();
void update_sensor_values();
void enable_motors(bool enable);
void run_control();
void send_motor_controls();
void set_name_value_pair(String name_, String value_);
void setReference(String str);

#endif //ARDUINO_PRIMARY_SHARED_H
