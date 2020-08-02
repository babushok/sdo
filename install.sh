#!/usr/bin/env bash

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

sudo apt install -y build-essential
sudo apt install -y cmake
sudo apt install -y qt5-default
sudo apt install -y libqt5webkit5-dev
sudo apt install -y qml-module-qtquick-controls
sudo apt install -y qml-module-qtquick-controls2
sudo apt install -y qtquickcontrols2-5-dev

$SCRIPTPATH/build.sh
