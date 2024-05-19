
# Get argument ROS_DISTRO from the build command or set default value
ARG ROS_DISTRO=iron
ARG VESSEL_ID=boat

# Use the official ROS 2 image
FROM ros:${ROS_DISTRO}-ros-base

# Install additional packages
 RUN apt-get update && apt-get install -y \
      pip
#     iputils-ping \
#     ros-iron-xacro

RUN pip install pyserial

# Copy the boid_ros_pkg directory into container ros2_ws/src
COPY ros_micro_bridge/. /ros2_ws/src/ros_micro_bridge

# Build the workspace
RUN . /opt/ros/${ROS_DISTRO}/setup.sh && \
    cd /ros2_ws && \
    colcon build

# Source ros when accessed via bash: (e.g. through `docker exec -it boid_1 /bin/bash`)
#RUN echo ". /entrypoint.sh" >> /root/.bashrc

# Set tasks to be run upon container startup
CMD . /opt/ros/${ROS_DISTRO}/setup.sh && \
    . /ros2_ws/install/setup.sh && \
    ros2 run ros_micro_bridge arduino_bridge_ros2 --ros-args --remap __ns:=/${VESSEL_ID}