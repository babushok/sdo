#!/usr/bin/env bash

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

rm -rf $SCRIPTPATH/bin
rm -rf $SCRIPTPATH/src/client/build
rm -rf $SCRIPTPATH/src/server/CMakeFiles
rm -f $SCRIPTPATH/src/include/common.h
rm -f $SCRIPTPATH/src/server/CMakeCache.txt
rm -f $SCRIPTPATH/src/server/cmake_install.cmake
rm -f $SCRIPTPATH/src/server/Makefile
