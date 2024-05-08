//
// Created by casper on 19-7-22.
//

#ifndef ARDUINO_PRIMARY_PIDMOTOR_H
#define ARDUINO_PRIMARY_PIDMOTOR_H

#include <PID_v1.h>

class PIDMotor {
    double reference_RPM = 0, measurement_RPM = 0, PID_dcMotor_output = 0;
    bool reversed = false;

    PID *pid;

public:
    PIDMotor(double Kp, double Ki, double Kd) {
        pid = new PID(&measurement_RPM, &PID_dcMotor_output, &reference_RPM, Kp, Ki, Kd, DIRECT);
    };

    ~PIDMotor() {
        free(pid);
    };

    void enable(bool enable) {
        if (enable) {
            pid->SetMode(AUTOMATIC);
        } else {
            pid->SetMode(DIRECT);
        }
    }

    void setReference(double reference) {
        if (reference >= 0) {
            reference_RPM = reference;
            reversed = false;
        } else {
            reference_RPM = -reference;
            reversed = true;
        }
    }

    double getReference() const {
        return reference_RPM;
    }

    double getMeasurement() const {
        return measurement_RPM;
    }

    bool getReversed() const {
        return reversed;
    }

    void setMeasurement(double measurement) {
        measurement_RPM = measurement;
    }

    double getMotorOutput() const {
        return PID_dcMotor_output;
    }

    void computePID() {
        pid->Compute();
    }
};

#endif //ARDUINO_PRIMARY_PIDMOTOR_H
