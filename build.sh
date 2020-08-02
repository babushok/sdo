#!/usr/bin/env bash

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

$SCRIPTPATH/clear_build.sh

cd $SCRIPTPATH/src/server

cmake .
cmake --build .

mkdir $SCRIPTPATH/src/client/build
cd $SCRIPTPATH/src/client/build

qmake -o Makefile ../client_qml.pro -spec linux-g++ CONFIG+=qtquickcompiler
make
