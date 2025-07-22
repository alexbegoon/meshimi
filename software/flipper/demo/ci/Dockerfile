FROM python:3.13-bookworm AS builder
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

# Install Dependencies
ENV PIP_ROOT_USER_ACTION=ignore
RUN apt-get update && apt-get install --no-install-recommends -y \
        curl wget g++ zip git ca-certificates pkg-config \
        libgpiod-dev libyaml-cpp-dev libbluetooth-dev libi2c-dev libuv1-dev \
        libusb-1.0-0-dev libulfius-dev liborcania-dev libssl-dev \
        libx11-dev libinput-dev libxkbcommon-x11-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/* \
    && pip install --no-cache-dir -U platformio \
    && mkdir /tmp/firmware