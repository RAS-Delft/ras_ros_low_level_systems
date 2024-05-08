# Low level Arduino code

This directory contains the C++ code for the Arduino that performs low-level tasks on the boat.

The code is organized as a [PlatformIO](https://platformio.org/) project. If you don't know how to work with PIO, see [the PIO readme](PIO_readme.md). There you will find out why it is used and how to use it.

## Functionality of the Arduino
The Arduino performs a few tasks

* Collect sensordata
  * Speeds of the thrusters
  * Accelerometer data
  * Gyroscope data
  * Magnetometer data
* Pass the sensor and state data to the computer over the serial connection
* Receive and parse the control signal coming from the computer over the serial connection
* Send the right control signal to the motor controllers
  * For this a PID loop runs with the motor speed as sensor input and control data as reference.
* Safety features
  * Turn off motors if no control signal has been received for a while.
