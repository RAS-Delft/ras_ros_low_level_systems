FROM python:3.10-slim

# Install additional packages
 RUN apt-get update && apt-get install -y \
    build-essential \
    libffi-dev \
    libssl-dev \
    pip \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN pip install -U platformio

COPY ras_main_micro_driver /ras_main_micro_driver

CMD     cd ras_main_micro_driver && \
        pio run -t upload