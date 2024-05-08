#include <Arduino.h>
#include "definitions.h"
#include "shared.h"
#include "PIDMotor.h"

// Bow thruster (feed-forward) control variables
double normalized_output_BOW;

/*** Assign Azipod start positions ***/
int zeroPoint_AZI_P = 7;  // forward configuration: 82  | reverse configuration: 124
int zeroPoint_AZI_SB = 12; // forward configuration: 95 | reverse configuration: 57
int reference_Angle_P = 0;
int reference_Angle_SB = 0;

/*** Motor control variables ***/
// Units: servo out signal is a number ranging from approximately 1200 (left) to 1500 (mid) to 1800 (right). Desired actuator angle is mapped onto this range before being passed to the servo.write function.
int motor_ESC_SignalOut_Bow = 1500;
int motor_ESC_SignalOut_P = 1500;
int motor_ESC_SignalOut_SB = 1500;

bool motors_enabled = false;

volatile int rpmcount_P = 0;
volatile int rpm_P = 0;
volatile unsigned long timestamp_last_full_rotation_P = micros();

volatile int rpmcount_SB = 0;
volatile int rpm_SB = 0;
volatile unsigned long timestamp_last_full_rotation_SB = micros();

volatile uint32_t last_interrupt_P = millis();
volatile uint32_t last_interrupt_SB = millis();

Servo ESC_P;
Servo ESC_SB;
Servo ESC_bow;
Servo AZI_P;
Servo AZI_SB;

PIDMotor PID_P(Kp_P, Ki_P, Kd_P);
PIDMotor PID_SB(Kp_SB, Ki_SB, Kd_SB);

/**
 * Function that enables or disables the motors and servos.
 * @param enable Whether to enable or disable.
 */
void enable_motors(bool enable) {
    motors_enabled = enable;

    PID_P.enable(enable);
    PID_SB.enable(enable);

    if (enable) {
        // Assign pins and set up ESC and AZI servos
        AZI_P.attach(pin_AZI_P);
        AZI_SB.attach(pin_AZI_SB);

        ESC_P.attach(pin_ESC_P);
        ESC_SB.attach(pin_ESC_SB);
        ESC_bow.attach(pin_ESC_bow);

        // ESC Initialization
        ESC_P.writeMicroseconds(1500);
        ESC_SB.writeMicroseconds(1500);
        ESC_bow.writeMicroseconds(1500);

        // Wait a bit to ensure the ESC's get a 'zero value' to initialize
        delay(600); 

    } else {
        AZI_P.detach();
        AZI_SB.detach();
        ESC_P.detach();
        ESC_SB.detach();
        ESC_bow.detach();
    }
}

/** Calculate RPMs of DC motors */
void rpm_interrupt_P() {
    rpmcount_P++;
    last_interrupt_P = millis();
    if (rpmcount_P == 16) { // There are 16 holes in the encoder, we want to have a value each 1 rounds
        rpm_P = round(60000000 / (micros() - timestamp_last_full_rotation_P));
        timestamp_last_full_rotation_P = micros();
        rpmcount_P = 0;
    }
}

void rpm_interrupt_SB() {
    rpmcount_SB++;
    last_interrupt_SB = millis();
    if (rpmcount_SB == 16) { // There are 16 holes in the encoder, we want to have a value each 1 rounds
        rpm_SB = round(60000000 / (micros() - timestamp_last_full_rotation_SB));
        timestamp_last_full_rotation_SB = micros();
        rpmcount_SB = 0;
    }
}

/**
 * Interprets and checks a reference string and sets the appropriate values
 * @param str Reference string
 */
void setReference(String str) {
    //Serial.println("[Debug] setReference: with arguments:'" + str + "'");

    // ---- Verify message contents
    int num_delimiters = find_num_delimiters(str, ';');
    if (num_delimiters == 4) { // number of expected inputs
        last_reference_received_timestamp = millis();

        // ---- Set P rps
        String str_section = "";
        int str_index = 0;
        while (str[str_index] != ';') {
            str_section.concat(str[str_index]);
            str_index++;
        }
        PID_P.setReference(str_section.toDouble());

        // ---- Set SB rps
        str_index++;
        str_section = "";
        while (str[str_index] != ';') {
            str_section.concat(str[str_index]);
            str_index++;
        }
        PID_SB.setReference(str_section.toDouble());

        // ---- Set bow pwm
        str_index++;
        str_section = "";
        while (str[str_index] != ';') {
            str_section.concat(str[str_index]);
            str_index++;
        }
        normalized_output_BOW = str_section.toDouble();

        // ---- Set Portside angle
        str_index++;
        str_section = "";
        while (str[str_index] != ';') {
            str_section.concat(str[str_index]);
            str_index++;
        }
        reference_Angle_P = str_section.toInt();

        // ---- Set Starboardside angle
        str_index++;
        str_section = "";
        while ((str[str_index] != '\n')&&(str_index<=str.length())) {
            str_section.concat(str[str_index]);
            str_index++;
        }
        reference_Angle_SB = str_section.toInt();

    } else {
        Serial.print("[Error] setReference(str) did not have the expected formatted input with str='"); Serial.print(str); Serial.println("'");
    }
}

void update_sensor_values() {
    // -------------------------------------- Update variables with current (volatile) RPM value
    PID_P.setMeasurement(rpm_P);
    PID_SB.setMeasurement(rpm_SB);

    // -------------------------------------- RPM TIMEOUT

    if (millis() - last_interrupt_P > ENCODER_RPS_TIMEOUT) {
        rpm_P = 0;
        timestamp_last_full_rotation_P = millis();
        rpmcount_P = 0;
    }

    if (millis() - last_interrupt_SB > ENCODER_RPS_TIMEOUT) {
        rpm_SB = 0;
        timestamp_last_full_rotation_SB = millis();
        rpmcount_SB = 0;
    }
}

void run_control() {
    PID_P.computePID();
    PID_SB.computePID();
}

#define minmax(num, min, max) ( (num)>(min) ? ((num)<(max) ? (num) : (max)) : (min) )

void send_motor_controls() {
    // ---- Set angles of servo's
    // There is a factor in here that compensates for gear ratio between servo and actual thruster angle
    if (motors_enabled) {
        AZI_P.write(90 + GEARRATIO_SERVO_AZIMUTH * (zeroPoint_AZI_P + reference_Angle_P));
        AZI_SB.write(90 + GEARRATIO_SERVO_AZIMUTH * (zeroPoint_AZI_SB + reference_Angle_SB));
    }

    // ---- Set DC motor values
    double normalized_output_BOW_out = normalized_output_BOW;
    double ESC_normalized_output_P_out = PID_P.getMotorOutput()/255.0;
    double ESC_normalized_output_SB_out = PID_SB.getMotorOutput()/255.0;

    // ---- Clip motor normalized output between [-1:1]
    normalized_output_BOW_out = minmax(normalized_output_BOW_out, -1, 1);
    ESC_normalized_output_P_out = minmax(ESC_normalized_output_P_out, -1, 1);
    ESC_normalized_output_SB_out = minmax(ESC_normalized_output_SB_out, -1, 1);

    // ---- Map Normalized Output [-1:1] to ESC controller input [1200:1800] and check minimum actuator threshold
    if ((normalized_output_BOW_out <= MIN_NORM_PWR_BOW_THRUSTER) &&
        (normalized_output_BOW_out >= -MIN_NORM_PWR_BOW_THRUSTER)) {
        motor_ESC_SignalOut_Bow = 1500; // Deactivate if below minimum threshold
        //Serial.println(" motor minimum threshold bow");
    } else {
        motor_ESC_SignalOut_Bow = map(normalized_output_BOW_out * 1000, -1000, 1000, 1000, 2000);
    }

    if ((PID_P.getReference() <= MIN_RPM_AFT_THRUSTER) && (PID_P.getReference() >= -MIN_RPM_AFT_THRUSTER)) {
        motor_ESC_SignalOut_P = 1500; // Deactivate if below minimum threshold
        //Serial.println(" motor minimum threshold p");
    } else {
        if (PID_P.getReversed()) {
            ESC_normalized_output_P_out *= -1;
        }
        motor_ESC_SignalOut_P = map(ESC_normalized_output_P_out * 1000, -1000, 1000, 1000, 2000);
    }

    if ((PID_SB.getReference() <= MIN_RPM_AFT_THRUSTER) && (PID_SB.getReference() >= -MIN_RPM_AFT_THRUSTER)) {
        motor_ESC_SignalOut_SB = 1500; // Deactivate if below minimum threshold
        //Serial.println(" motor minimum threshold sb");
    } else {
        if (PID_SB.getReversed()) {
            ESC_normalized_output_SB_out *= -1;
        }
        motor_ESC_SignalOut_SB = map(ESC_normalized_output_SB_out * 1000, -1000, 1000, 1000, 2000);
    }

    // ----Send signal to the motors
    if (motors_enabled) {
        ESC_bow.writeMicroseconds(motor_ESC_SignalOut_Bow);
        ESC_P.writeMicroseconds(motor_ESC_SignalOut_P);
        ESC_SB.writeMicroseconds(motor_ESC_SignalOut_SB);
    }
}
