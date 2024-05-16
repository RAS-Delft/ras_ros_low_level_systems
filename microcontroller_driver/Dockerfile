FROM python:3.8-slim

# Install necessary packages
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        git \
        && rm -rf /var/lib/apt/lists/*

# Install PlatformIO
RUN pip install -U platformio

# Set up a working directory
WORKDIR /app

# Copy your project files into the container
COPY . .

# Define entrypoint for running PlatformIO commands
ENTRYPOINT ["platformio"]
