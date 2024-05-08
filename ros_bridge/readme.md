# ROS-Arduino-Interconnection
As the title says, this part of the repository is dedicated to the software bridge that translates commands from a control system (probably in MATLAB or Python) to the Arduino, and sensor data back from Arduino to the control system.

## Functionality
This basic functionality of this script is to pass on actuation data from ROS to the arduino and periodically receive and publish telemetry.
Further functionalities that increase reliability, maintainability and ease-of-use are:
* Validating integrity of serial messages
* Receiving and displaying Arduino diagnostics
* Show diagnostics of the script itself
* Manage various timeouts, such as stopping the actuators when a reference signal stopped broadcasting for some time. 
* Catch and/or solve various system failures (e.g. reboot the Arduino if is suspected to have crashed)
* Automated selection of Arduino serial port
* Filter raw magnetometer data to form heading estimate which is then published.

## Using this package

This package is compatible with both ROS1 and ROS2. See usage instructions below. It assumes you:
* have a `catkin_ws` set-up when using ROS1 ([howto](http://wiki.ros.org/catkin/Tutorials/create_a_workspace)),
* or a `colcon` workspace called `ros2dev_ws` when using ROS2 ([howto](https://docs.ros.org/en/galactic/Tutorials/Beginner-Client-Libraries/Colcon-Tutorial.html)).

### ROS 1

To use this package, you either run the script directly, or you install it using `catkin` such that you can use `rosrun`
```shell
# Link repo directory source to destination catkin source
sudo ln -s ~/Documents/GitHub/RAS_TitoNeri/ras_low_level_bridge/ ~/catkin_ws/src/ras_low_level_bridge
cd ~/catkin_ws/
catkin_make
```

```shell
# Make sure to have the ROS environment correctly set up
rosrun ras_low_level_bridge arduino_bridge_ros1.py
```

This only works if ROS is properly sourced. This will be done automatically by the [set-ros-env.sh script](https://github.com/RAS-Delft/boat-daemon/blob/main/scripts/set-ros-env.sh). If not using the `ras` user or other circumstances, source manually. _Change details as needed._
```shell
source /opt/ros/noetic/setup.bash
source ~/catkin_ws/devel/setup.bash
export ROS_MASTER_URI=http://10.0.0.1:11311; export ROS_IP=10.0.0.10; export VESSEL_ID=RAS_TN_DB
```

### ROS 2
To use this package, you either run the script directly, or you install it using `colcon` such that you can use `ros2 run`
```shell
# Link repo directory source to destination catkin source
sudo ln -s ~/Documents/GitHub/RAS_TitoNeri/ras_low_level_bridge/ ~/ros2dev_ws/src/ras_low_level_bridge
cd ~/ros2dev_ws/
colcon build --packages-select ras_low_level_bridge
```

```shell
# Make sure to have the ROS environment correctly set up
ros2 run ras_low_level_bridge arduino_bridge_ros2
```

This only works if ROS is properly sourced. The ras `.bachrc` file using aforementioned script **sources ROS1**. Therefore, you need to do this manually. _Change details as needed._
```shell
# When using ras user, you can use the script in /home/ras/src/boat-daemon/scripts/
source set-ros2-env.sh

# If not, do manually
source /opt/ros/galactic/setup.bash
source ~/ros2dev_ws/install/setup.bash
export VESSEL_ID=RAS_TN_DB
```

### Manual
You can also run the scripts of this package manually. Make sure the dependencies are installed (either through `apt` or `pip`, globally or in venv). Then run

```shell
python3 arduino_bridge_ros1.py # For ROS 1
python3 arduino_bridge_ros2.py # For ROS 2
```
