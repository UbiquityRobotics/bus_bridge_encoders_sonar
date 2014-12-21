// Copyright (c) 2014 by Wayne C. Gramlich.  All rights reserved.

// The following tests are implemented by this code:
#define TEST_BUS_INPUT 1
#define TEST_BUS_OUTPUT 2
#define TEST_BUS_ECHO 3
#define TEST_BUS_INTERRUPT_INPUT 4
#define TEST_BUS_COMMAND 5

// Set TEST to one of the possible tests:
#define TEST TEST_BUS_INPUT

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
NULL_UART null_uart;
AVR_UART0 debug_uart;
AVR_UART1 bus_uart;
Bus bus(&bus_uart, &debug_uart);
Bus_Bridge_Encoders_Sonar bus_bridge_encoders_sonar(33);

Bus_Bridge_Encoders_Sonar::Bus_Bridge_Encoders_Sonar(UByte address) {
  _address = address;
  _led = (Logical)0;
}

Logical Bus_Bridge_Encoders_Sonar::led_get() {
  return _led;
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
	bus->ubyte_put(led);
      }
      break;
    }
    case 1: {
      // led_set:
      Logical led = (Logical)bus->ubyte_get();
      if (execute_mode) {
	bus_bridge_encoders_sonar.led_set(led);
      }
      break;
    }
  }
  return 0;
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
      //Serial.write(frame);
      if (frame == '_') {
        //Serial.write("\r\n");
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
          //Serial.write(echo_frame);
          if ((echo_frame & 1) == 0) {
            digitalWrite(LED, LOW);
          } else {
            digitalWrite(LED, HIGH);
          }
        } else {
          //Serial.write('!');
        }

        if (echo_frame == '_') {
          //Serial.write('\r');
          //Serial.write('\n');
        }
      }
      break;
    }
    case TEST_BUS_OUTPUT: {
      // This test will just output characters to the debugging port
      // with no interraction with the bus:

      // Start with with *character* being static:
      static Character character = '@';

      // Just in case, make sure *character* is between '@' and '_':
      if (character < '@' || character > '_') {
	character = '@';
      }

      // Output *character* to the *avr_uart0*:
      debug_uart.frame_put((UShort)character);

      // Output any needed CRLF, and increment *character*:
      if (character == '_') {
	debug_uart.string_print((Character *)"\r\n");
	character = '@';
      } else {
	character += 1;
      }

      // Slow things down a little:
      delay(100);

      break;
    }
    case TEST_BUS_INPUT: {
      // This test will input *frame*'s from the *bus* and echo them
      // to *debug_uart*:

      // Get a *frame* from the bus:
      UShort frame = bus.frame_get();

      // Print it out for debugging:
      debug_uart.frame_put(frame & 0x7f);

      // Tack on a CRLF when we get an '_':    
      if (frame == (UShort)'_') {
	debug_uart.string_print((Character *)"\r\n");
      }

      // Light the *LED* using the least significant bit of *frame*:
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
  // The standard frequency for an AVR Arduino is 16MHz:
  static const UInteger frequency = (UInteger)16000000L;

  // Initalize the *debug_uart*:
  debug_uart.begin(frequency, 115200L, (Character *)"8N1");

  // Announce that we have got *debug_uart* working:
  debug_uart.string_print((Character *)"\r\nbes:\r\n");

  // Turn *LED* on:
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // Get *bus_uart* talking to the bus:
  bus_uart.begin(frequency, 500000L, (Character *)"9N1");

  // Force the standby pin on the CAN transeciever to low to force it
  // into active mode:
  pinMode(BUS_STANDBY, OUTPUT);
  digitalWrite(BUS_STANDBY, LOW);

  // Enable/disable interrupts based on *TEST*:
  switch (TEST) {
    case TEST_BUS_INPUT:
    case TEST_BUS_ECHO:
      bus.interrupts_disable();
      break;
    case TEST_BUS_OUTPUT:
      break;
    case TEST_BUS_INTERRUPT_INPUT:
    case TEST_BUS_COMMAND:
      // Force into interrupt mode:
      bus.interrupts_enable();
      break;
  }
}

