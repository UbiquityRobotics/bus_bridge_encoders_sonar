#ARDUINO_DIR = /usr/share/arduino
ARDUINO_DIR = ../Arduino
ARDMK_DIR = ../Arduino-Makefile
ALTERNATE_CORE = ubiquity/avr
ALTERNATE_CORE_PATH = $(ARDUINO_DIR)/hardware/$(ALTERNATE_CORE)
BOARD_TAG = bus_bridge_encoders_sonar
ARDUINO_PORT = /dev/ttyUSB0
ARDUINO_LIBS := . ../bus_slave ../bus_common
USER_LIB_PATH = $(realpath .)
MCU=atmega328p
F_CPU = 16000000L
VARIANT = standard
AVRDUDE_ARD_PROGRAMMER = arduino
AVRDUDE_ISP_BAUDRATE = 115200
AVRDUDE_ARD_BAUDRATE = 115200
BUS_CODE_GENERATOR = ../bus_slave/bus_code_generator.py

include $(ARDMK_DIR)/Arduino.mk

bus_bridge_encoders_sonar.ino: Bus_Bridge_Encoders_Sonar_Local.h

Bus_Bridge_Encoders_Sonar_Local.h Bus_Bridge_Encoders_Sonar_Local.cpp: \
    Bus_Bridge_Encoders_Sonar.xml
	$(BUS_CODE_GENERATOR) Bus_Bridge_Encoders_Sonar.xml

