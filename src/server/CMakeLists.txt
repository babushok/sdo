cmake_minimum_required(VERSION 3.10)

project(sdo VERSION 0.0.5)

set(CHANNEL_COUNT 8)

set(SERVER_APP_NAME "server_cli")

set(SERVER_PATH "/tmp/${SERVER_APP_NAME}.socket")

include_directories("../include")

configure_file(${PROJECT_SOURCE_DIR}/../include/common.h.in ${PROJECT_SOURCE_DIR}/../include/common.h)

add_executable(${SERVER_APP_NAME} ${PROJECT_SOURCE_DIR}/main.cpp)

add_executable(client_cli ${PROJECT_SOURCE_DIR}/../client/client_cli.cpp)

set_target_properties(${SERVER_APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../bin)

set_target_properties(client_cli PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../bin)

set_target_properties(${SERVER_APP_NAME} client_cli PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
        )