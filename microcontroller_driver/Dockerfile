ARG ROS_DISTRO=iron

# Use the official ROS 2 image
FROM ros:${ROS_DISTRO}-ros-base

# Install necessary packages
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        git \
        pip \
        && rm -rf /var/lib/apt/lists/*

# Install PlatformIO
RUN pip install -U platformio



# Copy your project files into the container
COPY ras_main_micro_driver /ras_main_micro_driver

# Set up a working directory
WORKDIR /ras_main_micro_driver

CMD [ "pio run -t upload --upload-port /dev/tty" ]
