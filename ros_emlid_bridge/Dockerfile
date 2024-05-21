
# Get argument ROS_DISTRO from the build command or set default value
ARG ROS_DISTRO=iron

# Use the official ROS 2 image
FROM ros:${ROS_DISTRO}-ros-base

# Copy the boid_ros_pkg directory into container ros2_ws/src
COPY reach_ros_node/. /ros2_ws/src/reach_ros_node

# Build the workspace
RUN . /opt/ros/${ROS_DISTRO}/setup.sh && \
    cd /ros2_ws && \
    colcon build

# Set tasks to be run upon container startup
CMD . /opt/ros/${ROS_DISTRO}/setup.sh && \
    . /ros2_ws/install/setup.sh && \
    ros2 run reach_ros_node nmea_tcp_driver --ros-args -p host:=192.168.2.15 -p port:=9001 -r __ns:=/RAS_TN_DB -r tcpfix:=telemetry/gnss/fix -r tcpvel:=telemetry/gnss/tcpvel -r tcptime:=telemetry/gnss/tcptime
