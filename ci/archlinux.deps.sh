#!/bin/bash -eux

source ci/common.setup.sh

PACKAGES=
if [[ ! -f /usr/lib/libjack.so ]]; then
  PACKAGES+=" jack2 "
fi

$SUDO pacman -S --noconfirm --needed \
   $PACKAGES \
   cmake ninja gcc llvm clang boost \
   qt5-base qt5-imageformats qt5-websockets qt5-serialport qt5-declarative qt5-tools icu \
   ffmpeg portaudio lv2 suil lilv sdl2 alsa-lib \
   avahi fftw bluez-libs

source ci/common.deps.sh
