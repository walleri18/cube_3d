SRC := LinuxRaspberryCube.cpp joystick.cc
CPP_FLAGS := -lwiringPi
RELEASE_DIR := ./release
OUTPUT := ${RELEASE_DIR}/joymx

all: ${OUTPUT}

create-release-dir:
	mkdir -p ${RELEASE_DIR}

${OUTPUT}: create-release-dir
	g++ ${SRC} -o ${OUTPUT} ${CPP_FLAGS}
