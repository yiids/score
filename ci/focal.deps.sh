#!/bin/bash -eux

source ci/common.setup.sh

echo 'debconf debconf/frontend select Noninteractive' | $SUDO debconf-set-selections

$SUDO apt-get update -qq
$SUDO apt-get install -qq --force-yes software-properties-common wget

$SUDO apt purge --auto-remove cmake
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | $SUDO tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
$SUDO apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'

$SUDO apt-get install -qq --force-yes \
    --allow-change-held-packages \
    --allow-downgrades \
    --allow-remove-essential \
    --allow-unauthenticated \
     binutils gcc-10 g++-10 clang-10 clang++-10 \
     libasound-dev \
     ninja-build cmake \
     libfftw3-dev \
     libsuil-dev liblilv-dev lv2-dev \
     qtbase5-dev qtdeclarative5-dev libqt5serialport5-dev libqt5websockets5-dev libqt5opengl5-dev \
     qtbase5-private-dev qtdeclarative5-private-dev \
     libbluetooth-dev libsdl2-dev libsdl2-2.0-0 \
     libglu1-mesa-dev libglu1-mesa libgles2-mesa-dev \
     libavahi-compat-libdnssd-dev libsamplerate0-dev \
     portaudio19-dev \
     libavcodec-dev libavdevice-dev libavutil-dev libavfilter-dev libavformat-dev libswresample-dev

source ci/common.deps.sh
