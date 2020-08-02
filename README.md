# Client/Server Device Project
The software implementation both client and server simulating device management. The device is a universal multichannel multimeter (2...n channels of voltage measurement). The project is the result of the [test task](https://github.com/babushok/sdo/blob/master/docs/task.pdf).

## Getting Started
These instructions will get you a copy of the project up and running on your local machine for evaluating and compliance purposes. See Installation for notes on how to install dependencies and build the project.

## Requirements
The project was successfully built and tested on Ubuntu 18.04 and 20.04. To compile and build the project the following packages are required: build-essential, cmake, qt5-default, libqt5webkit5-dev, qml-module-qtquick-controls, qml-module-qtquick-controls2, qtquickcontrols2-5-dev.

## Installation

- Clone this repo to your local machine using `https://github.com/babushok/sdo`
- To install all depencies and build the project, run at the repo root:
```sh
$ ./install.sh
```
- To build the one project, run script at the repo root:
```sh
$ ./build.sh
```
- To clean up repo from the artifacts, run script at the repo root:
```sh
$ ./clear_build.sh
```

## Usage
The executable files are placed in the repo folder `./bin` after building:
```sh
./bin/client_cli - console client
./bin/client_qml - qml client
./bin/server_cli - console server
```
To run the simulating device managment project, the server is started first:

```sh
$ ./bin/server_cli
```
![Recordit GIF](http://g.recordit.co/z8rJ0Uu2PX.gif)
----
Then any of the clients is launched for interaction:
> Console client
```sh
$ ./bin/client_cli
```
![Recordit GIF](http://g.recordit.co/Vy7xWqkzaN.gif)

Available console client commands:
| Command      | Parameters       | Description           |
| :----------- |:-----------------|:----------------------|
| start_measure| channelN         |Channel measuring start|
| stop_measure | channelN         |Channel measuring stop |
| get_status   | channelN         |Get channel state      |
| get_result   | channelN         |Get channel voltage value|
| set_range    | channelN, rangeM |Set channel voltage measuring range|

The channel is specified as a channel[integer from 0 to N-1], for example, "channel0",
"channel1",..., "channel7". Measurement channel range - range[integer from 0
to M-1], where M is 4: "range0", "range1", "range2", "range3".

----

> QML client
```sh
$ ./bin/client_qml
```
![Recordit GIF](http://g.recordit.co/60RrjIuCmQ.gif)

## Repo contents

    /
    ├── docs                                 # Documentation files
    │   └── task.pdf                         # Test task document
    ├── src                                  # Source files
    │   ├── client                           # Client apps source files
    │   │   ├── include                      # Client apps header files 
    │   │   │   ├── client.h                 # Client header-only class unix domain socket based
    │   │   │   ├── multimeter.h             # Channel and multimeter classes interface 
    │   │   │   └── multimeter_model.h       # Multimeter_model class interface based on 
    │   │   │                                # QAbstractListModel class
    │   │   ├── qml                          # Client app QML Documents
    │   │   │   ├── ColorButton.qml          # Custom button QML object type
    │   │   │   ├── ColorButton2.qml
    │   │   │   ├── connect_failed.qml       # QML window object for error displaying
    │   │   │   ├── main.qml                 # Main QML window object
    │   │   │   └── Multimeter.qml           # Custom QML Object for the multimeter model 
    │   │   │                                # encapsulation
    │   │   ├── client_cli.cpp               # Console client main source file
    │   │   ├── client_qml.pro               # QML client Qt Creator project file
    │   │   ├── main.cpp                     # QML client main source file
    │   │   ├── multimeter.cpp               # Channel and multimeter classes implementation
    │   │   ├── multimeter_model.cpp         # Multimeter model class implementation
    │   │   ├── qml.qrc                      # Resource Collection File
    │   │   └── qtquickcontrols2.conf        # Qt Quick Controls Configuration File
    │   │
    │   ├── include                          # Common header files
    │   │   ├── common.h.in                  # CMake file to configure the header common.h
    │   │   ├── error.h                      # Error header-only class
    │   │   └── log.h                        # Log header-only class wrapper for syslog
    │   │
    │   └── server                           # Server app source files
    │       ├── include                      # Server app header files 
    │       │   ├── channel.h                # Channel header-only class
    │       │   ├── command.h                # Command header-only class (pattern command)
    │       │   ├── multimeter.h             # Multimeter header-only class
    │       │   └── server.h                 # Server header-only class Unix domain socket based
    │       ├── CMakeLists.txt               # CMake configuration file
    │       └── main.cpp                     # Server main source file
    ├── build.sh                             # Build the project script
    ├── clear_build.sh                       # Clean up repo from the artifacts script
    ├── install.sh                           # Install all dependencies and build the project script
    └── README.md                            # This document
   
## Technologies
The project is made using С++17 standard, STL and Unix domain sockets data communications endpoints. The QML-client was developed in the cross-platform IDE Qt Creator using the Qt Quick framework. It was designed using the model/view architecture to manage the relationship between data and the way it is presented to the user. This architecture gives greater flexibility to customize the presentation of items.

## Project setup
The number of server channels and the server application name is set in the CMake configuration file `./src/server/CMakeLists.txt`:
```C++
set(CHANNEL_COUNT 5)

set(SERVER_APP_NAME "server_cli")
```
The client determines the number of server channels in runtime.

## Extend command list
To provide a simple implementation of the command list extension the server was designed using a command pattern and callback functions. To add a new command you need to insert a pair <command string, function reference> into the map in multimeter class `./src/server/include/multimeter.h`:
```C++
const std::map <std::string, func_t> commands_ = {{"command", &function}};

const std::map <std::string, func_ex_t> commands_ex_ = {{"command", &function}};
```
