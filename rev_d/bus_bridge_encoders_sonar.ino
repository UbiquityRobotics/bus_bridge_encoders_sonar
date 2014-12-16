// Copyright (c) 2014 by Wayne C. Gramlich.  All rights reserved.

// The following tests are implemented by this code:
#define TEST_BUS_INPUT 1
#define TEST_BUS_ECHO 2
#define TEST_BUS_INTERRUPT_INPUT 3
#define TEST_BUS_COMMAND 4

// Set TEST to one of the possible tests:
#define TEST TEST_BUS_COMMAND

// Watch-out the code for SerialHardwared.cpp has been modified to
// notice this #define.  It causes the 8-ibt interrupt driver for
// UART1 to be disabled:
#define UART1_DISABLED

#define BUS_BAUD_500KBPS 3 // Fosc=16MHz & U2X1=1
#define LED 13
#define BUS_STANDBY A4

#include "Bus.h"
#include <Bus_Bridge_Encoders_Sonar.h>

// Object variables:
Bus bus;
Bus_Bridge_Encoders_Sonar bus_bridge_encoders_sonar(33);

Bus_Bridge_Encoders_Sonar::Bus_Bridge_Encoders_Sonar(UByte address) {
  _address = address;
  _led = (Logical)0;
}

void Bus_Bridge_Encoders_Sonar::led_set(Logical led) {
  _led = led;
  digitalWrite(13, led);
}

UByte command_process(Bus *bus, UByte command, Logical execute_mode) {
  switch (command) {
    // Bus_Bridge_Encoders_Sonar
    case 0: {
      // led_get:
      if (execute_mode) {
	Logical led = bus_bridge_encoders_sonar.led_get();
	bus->logical_put(led);
      }
      break;
    }
    case 1: {
      // led_set:
      Logical led = bus->logical_get();
      if (execute_mode) {
	bus_bridge_encoders_sonar.led_set(led);
      }
      break;
    }
  }
  return 0;
}

Logical Bus_Bridge_Encoders_Sonar::led_get() {
  return _led;
}

void loop() {
  switch (TEST) {
    case TEST_BUS_COMMAND: {
      bus.slave_mode(0x21, command_process);
      break;
    }
    case TEST_BUS_INTERRUPT_INPUT: {
      // Read the next *frame* from the bus:
      UShort frame = bus.frame_get();

      // Echo *frame* to the debugging port with an occasional CRLF:
      Serial.write(frame);
      if (frame == '_') {
        Serial.write("\r\n");
      }

      // Set *LED* to have the LSB of *frame*:
      if ((frame & 1) == 0) {
        digitalWrite(LED, LOW);
      } else {
        digitalWrite(LED, HIGH);
      }
      break;
    }
    case TEST_BUS_ECHO: {
      if (bus.can_receive()) {
        UShort receive_frame = bus.frame_get();
        UShort send_frame = receive_frame;
        bus.frame_put(send_frame);
        UShort echo_frame = bus.frame_get();
    
        if (receive_frame == echo_frame) {
          Serial.write(echo_frame);
          if ((echo_frame & 1) == 0) {
            digitalWrite(LED, LOW);
          } else {
            digitalWrite(LED, HIGH);
          }
        } else {
          Serial.write('!');
        }

        if (echo_frame == '_') {
          Serial.write('\r');
          Serial.write('\n');
        }
      }
      break;
    }
    case TEST_BUS_INPUT: {
      //Serial.write("A:");
      //Serial.print(UCSR1A, HEX);
      //Serial.write(' ');
      //Serial.write("B:");
      //Serial.print(UCSR1B, HEX);
      //Serial.write(' ');
      //Serial.write("C:");
      //Serial.print(UCSR1C, HEX);
      //Serial.write("\r\n");

      // Get a *frame* from the bus:
      UShort frame = bus.frame_get();

      // Print it out for debugging:
      Serial.write(frame);
      if (frame == '_') {
        Serial.write("\r\n");
      }

      // Light the *LED* using the LSB of *frame*:
      if ((frame & 1) == 0) {
        digitalWrite(LED, LOW);
      } else {
        digitalWrite(LED, HIGH);
      }
      break;
    }
  }
}

void setup() {
  // Initalize the debugging port:
  Serial.begin(115200);
  Serial.print("\r\nbbes:\r\n");

  // Turn *LED* on:
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // Force the standby pin on the CAN transeciever to low to force it
  // into active mode:
  pinMode(BUS_STANDBY, OUTPUT);
  digitalWrite(BUS_STANDBY, LOW);

  // Enable/disable interrupts based on *TEST*:
  switch (TEST) {
    case TEST_BUS_ECHO:
    case TEST_BUS_INPUT:
      bus.interrupts_disable();
      break;
    case TEST_BUS_INTERRUPT_INPUT:
    case TEST_BUS_COMMAND:
      // Force into interrupt mode:
      bus.interrupts_enable();
      break;
  }
}

