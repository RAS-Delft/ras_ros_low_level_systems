#include <Arduino.h>
#include <Streaming.h>
#include "definitions.h"
#include "shared.h"

String serial0InputString = ""; // TX/RX - USB

void set_system_variables(String str);

/**
  Note on Serial communication

  1) From higher level components (e.g. PC, Nuc, Raspberry Pi) through SERIAL0
    Serial connection to the arduino currently distinguishes various message types, indicated by the first character(s).
    Distuinguished received message types are:
      - "r" - Reference for actuators   "r;rpm_P;rpm_SB;pwm_bow;theta_P;theta_SB"
      - "s" - Set variable              "s;varname1;value1;varname2;value2"
      - "g" - Get variable              "g;varname1;varname2;varname3"
      - "c" - Run command               "c;saveCalibEEPROM"
      Where the identicator is followed by the arguments and ended with a newline \n character.

    Distuinguished outbound message types are:
      - "t" - Telemetry                 "t;telemvalue1;telemvalue2;telemvalue3..."
      - "["   Diagnostics               "[Info] Battery voltage = 11.90V"
                                        "[Debug] variable1 = "+var1.tostring();
                                        "[Warn] unexpected value in function: xxx"
                                        "[Error] Function xxx has faulty input arguments and was caught"
                                        "[Fatal] Engine 1 caught fire"
    Where the identicator is followed by the arguments and ended with a newline \n character.
**/

/**
   Send telemetry data to PC
*/
void serial_send_telemetry() {
    String IMU_data;
    if (IMU_enabled) {
        // Possible vector values can be:
        // - VECTOR_ACCELEROMETER - m/s^2
        // - VECTOR_MAGNETOMETER  - uT
        // - VECTOR_GYROSCOPE     - rad/s
        imu::Vector<3> accelerometer = getIMUVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        imu::Vector<3> gyro = getIMUVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
        imu::Vector<3> magneto = getIMUVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);

        IMU_data = (String)accelerometer.x() + ';' + (String)accelerometer.y() + ';' + (String)accelerometer.z() + ';' + (String)gyro.x() + ';' + (String)gyro.y() + ';' + (String)gyro.z() + ';' + (String)magneto.x() + ';' + (String)magneto.y() + ';' + (String)magneto.z();
    } else {
        IMU_data = "-1;-1;-1;-1;-1;-1;-1;-1;-1";
    }

    // message = [character_t; dcmotor_P_reference (RPM) ;dcmotor_SB_reference (RPM) ;normalized_output_BOW (-1:+1) ;reference_Angle_P (deg) ;reference_Angle_SB (deg) ;dcmotor_P_estimated (RPM) ;dcmotor_SB_estimated (RPM) ;dcmotor_P_Output (0-255) ;dcmotor_SB_Output (0-255) ;arduinoInternalClock (ms) ;IMU_accell_x (m/s) ;IMU_accell_y (m/s) ;IMU_accell_z (m/s) ;IMU_gyro_x ;IMU_gyro_y ;IMU_gyro_z ;IMU_magneto_x ;IMU_magneto_y ;IMU_magneto_z ;motor_ESC_SignalOut_P (1200-1800Hz) ;motor_ESC_SignalOut_SB (1200-1800Hz) /endOfLineCharacter]
    Serial << 't' << ';'
           << _FLOAT(PID_P.getReference(), 2) << ';' << _FLOAT(PID_SB.getReference(), 2) << ';' << _FLOAT(normalized_output_BOW, 2) << ';'
           << reference_Angle_P << ';' << reference_Angle_SB << ';'
           << _FLOAT(PID_P.getMeasurement(), 2) << ';' << _FLOAT(PID_SB.getMeasurement(), 2) << ';'
           << _FLOAT(PID_P.getMotorOutput(), 2) << ';' << _FLOAT(PID_SB.getMotorOutput(), 2) << ';'
           << millis() << ';'
           << IMU_data << ';'
           << _FLOAT(motor_ESC_SignalOut_P, 2) << ';' << _FLOAT(motor_ESC_SignalOut_SB, 2) << endl;

    diagnostics_tracker_count_telemetry++;
}

/**
   Process Serial input from Serial0 (USB)
   - Collects chars into the full message
   - Determines meaning of this message (e.g. reference, set parameter, or request for info)
   - Forwards contents to the right function to be processed
   - Keeps reading and handling serial buffer until it's empty, before continuing to the next main loop of the microcontroller
 **/
void serialEvent() {
    while (Serial.available()) {
        char inChar = (char) Serial.read();
        if (inChar != '\n') { // Ignore end of line characters
            serial0InputString += inChar;
        } else { // -- Handle the message contents

            if (serial0InputString[0] == 'r') {           // -------- Reference set
                setReference(serial0InputString.substring(1, serial0InputString.length()));

            } else if (serial0InputString[0] == 's') {    // -------- Set system variable
                set_system_variables(serial0InputString);

            } else if (serial0InputString[0] == 'g') {    // -------- Get system information
                if (serial0InputString.substring(1, serial0InputString.length()) == ";IMU_calibration_rating") {

                } else if (serial0InputString.substring(1, serial0InputString.length()) == ";BatteryVoltage") {

                } else {
                    Serial.println("[Error] serialEvent()/getParameter: Parameter name not recognised - Ignored");
                }

            } else if (serial0InputString[0] == 'c') {    // -------- Run command
                Serial.println("Run-command-FNC with arguments:'" + serial0InputString.substring(1, serial0InputString.length()) + "'" + "CALLBACK NOT IMPLEMENTED YET");
                if (serial0InputString.substring(1, serial0InputString.length()) == ";saveCalibEEPROM") {
                    saveCalibEEPROM();
                } else {
                    Serial.println("[Error] serialEvent()/runCommand: Command not recognised - Ignored");
                }

            } else {                                      // -------- Invallid command
                Serial.println("[Error] serialEvent(): Command ignored as type was not identified from first character msg='" + serial0InputString + "'");
            }
            serial0InputString = "";
        }
    }
}

/**
   set_system_variables finds name value pairs in the argument string and attempts to set the appropriate system parameters to them
   @param str String with command to change parameter with identifier and name value pairs (e.g. "r;zeroPoint_AZI_P;91;zeroPoint_AZI_P;85" )
*/
void set_system_variables(String str) {
    int delimiters_found_counter = find_num_delimiters(str, ';'); // counts delimiters found to validate message (it should be an equal number) and determine how many pairs are given

    // ---- Check if the amount of delimiters is sensible (should be divisible by 2 to have 1 message identifier and n pairs)
    if ( (delimiters_found_counter % 2) != 0) { // Incorrect amount of delimiters
        Serial.println("[Error] An uneven amount of delimiters detected. The set-parameter option requires an even amount of inputs to form name-value pairs. None of the inputs set");
    } else { // Correct amount of delimiters
        int string_index_current = 0; // index for cycling through the sting

        // ---- Move index past the message type identifier
        // This is usually a single iteration, as the identifier is likely a single char, but this allows the identifier to be longer as well.
        while (true) {
            string_index_current++;
            if (string_index_current >= str.length()) { // Stop if at end of string
                break;
            }
            if (str.charAt(string_index_current) == ';') { // Stop at first delimiter
                break;
            }
        }

        // ---- Read through the string to handle all name value pairs
        for (int i = 1; i <= delimiters_found_counter / 2; i++) {
            String nameSetStr = "";
            String valueSetStr = "";

            // ---- Read name
            while (true) {
                string_index_current++;
                if (string_index_current >= str.length()) { // Stop if at end of string
                    break;
                }
                if (str.charAt(string_index_current) != ';') { // Concatenate until at delimiter
                    nameSetStr.concat(str.charAt(string_index_current));
                } else {
                    break;
                }
            }

            // ---- Read value
            while (true) {
                string_index_current++;
                if (string_index_current >= str.length()) { // Stop if at end of string
                    break;
                }
                if (str.charAt(string_index_current) != ';') { // Concatenate until at delimiter
                    valueSetStr.concat(str.charAt(string_index_current));
                } else {
                    break;
                }
            }

            // ---- Set the found name value pair
            bool valid_parameter_combination = true;
            if (nameSetStr == "zeroPoint_AZI_P") {
                zeroPoint_AZI_P = valueSetStr.toInt();
            } else if (nameSetStr == "zeroPoint_AZI_SB") {
                zeroPoint_AZI_SB = valueSetStr.toInt();

            } else if (nameSetStr == "IMU_enabled") {
                if ((valueSetStr == "true") || (valueSetStr == "True") || (valueSetStr == "TRUE") || (valueSetStr == "1")) {
                    IMU_enabled = true;
                } else if ((valueSetStr == "false") || (valueSetStr == "False") || (valueSetStr == "FALSE") || (valueSetStr == "0")) {
                    IMU_enabled = false;
                } else {
                    Serial.println("[Error] Function set_name_value_pair: input parameter " + nameSetStr + " not identifyable as boolean");
                }
            } else {
                valid_parameter_combination = false;
            }

            // ---- Report result
            if (valid_parameter_combination) {
                Serial.println("[Info] Parameter " + nameSetStr + " set to " + valueSetStr);
            } else {
                Serial.println("[Error] Function set_name_value_pair: could not match name '" + nameSetStr + "' with a system parameter for value '" + valueSetStr + "'");
            }
        }
    }
}

/** Finds occurrences of a character within a string
    @param str String to evaluate
    @param delimiter Char to look for
*/
int find_num_delimiters(String str, char delimiter) {
    int delimiters_found_counter = 0;

    // ---- Find the number of delimiters
    for (int i = 0; i < str.length(); i++) {
        if (str.charAt(i) == delimiter) {
            delimiters_found_counter++;
        }
    }
    return delimiters_found_counter;
}


/*
  void serialEvent1() {
  while (Serial1.available()) {
    Serial.print(Serial1.read());
  }
  }
*/
