QT += quick quickcontrols2
CONFIG += c++17 release

QMAKE_CXXFLAGS += -std=c++17

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    multimeter.cpp \
    multimeter_model.cpp

RESOURCES += qml.qrc

INCLUDEPATH = ../include

CONFIG(debug, debug|release) {
    ARTIFACT_DIR = debug
    DESTDIR = ARTIFACT_DIR
}

CONFIG(release, debug|release) {
    ARTIFACT_DIR = release
    DESTDIR = ../../../bin
}

OBJECTS_DIR = $$ARTIFACT_DIR/.obj
MOC_DIR = $$ARTIFACT_DIR/.moc
RCC_DIR = $$ARTIFACT_DIR/.qrc
UI_DIR = $$ARTIFACT_DIR/.u

HEADERS += \
    ../include/common.h \
    ../include/error.h \
    ../include/log.h \
    include/client.h \
    include/multimeter.h \
    include/multimeter_model.h
