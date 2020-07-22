cmake_minimum_required(VERSION 3.10)

project(sdo VERSION 0.0.5)

set(CHANNEL_COUNT 5)

set(SERVER_APP_NAME "bis_server")

set(SERVER_PATH "/tmp/${SERVER_APP_NAME}.socket")

configure_file(${PROJECT_SOURCE_DIR}/server/common.h.in ${PROJECT_SOURCE_DIR}/server/common.h)

add_executable(${SERVER_APP_NAME} ${PROJECT_SOURCE_DIR}/server/main.cpp)

add_executable(client_cli ${PROJECT_SOURCE_DIR}/server/client_cli.cpp)

set_target_properties(${SERVER_APP_NAME} client_cli PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
        )
