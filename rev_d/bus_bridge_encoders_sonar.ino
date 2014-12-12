// The following tests are implemented by this code:
#define TEST_BUS_INPUT 1
#define TEST_BUS_ECHO 2
#define TEST_BUS_INTERRUPT_INPUT 3
#define TEST_BUS_COMMAND 4

// Set TEST to one of the possible tests:
#define TEST TEST_BUS_INTERRUPT_INPUT

// Watch-out the code for SerialHardwared.cpp has been modified to
// notice this #define.  It causes the 8-ibt interrupt driver for
// UART1 to be disabled:
#define UART1_DISABLED

#define BUS_BAUD_500KBPS 3 // Fosc=16MHz & U2X1=1
#define LED 13
#define BUS_STANDBY A4

#include "Bus.h"
#include <Bus_Bridge_Encoders_Sonar.h>

// typedef's go here:
typedef unsigned char Bool8;
typedef unsigned short Frame;
typedef struct Frame_Buffer__Struct *Frame_Buffer;
typedef unsigned short UInt16;

#if TEST == TEST_BUS_COMMAND || TEST == TEST_BUS_INTERRUPT_INPUT

// Object variables:
Bus bus;
Bus_Bridge_Encoders_Sonar bus_bridge_encoders_sonar(33);

Bus_Bridge_Encoders_Sonar::Bus_Bridge_Encoders_Sonar(UByte address) {
  _address = address;
  _led = (Logical)0;
}

#endif // TEST == TEST_BUS_COMMAND || TEST == TEST_BUS_INTERRUPT_INPUT

#if TEST == TEST_BUS_COMMAND

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

void loop() {
  bus.slave_mode(33, command_process);
}

#endif // TEST == TEST_BUS_COMMAND

#if TEST == TEST_BUS_INTERRUPT_INPUT

// The *TEST_BUS_INTERRUPT_INPUT* test verifies that the bus interrupt
// routine is working correctly.

void loop() {
  // Read the next *frame* from the bus:
  Frame frame = bus.frame_get();

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
}

#endif // TEST == TEST_BUS_INTERRUPT_INPUT

// The code from here on down is all polling based.  It is useful
// only for the initial hardware debugging.

#if TEST == TEST_BUS_INPUT || TEST == TEST_BUS_ECHO

void Bus_Serial__initialize(Byte baud_lsb) {
    // Clear out the three USART1 control/status registers:
    UCSR1A = 0;
    UCSR1B = 0;
    UCSR1C = 0;

    // Set the UART1 into Asynchronous mode (set PC:UMSEL11,PC:UMSEL10) = (0,0):
    //UCSR1C &= ~(1<<UMSEL11);
    //UCSR1C &= ~(1<<UMSEL10);

    // Set the UART1 into no parity mode (set PC:UPM11,PC:UPM10) = (0,0):
    //UCSR1C &= ~(1<<UPM11);
    //UCSR1C &= ~(1<<UPM10);

    // Set the UART1 to have 1 stop bit (Set PC:USBS1) = (0):
    //UCSR1C &= ~(1<<USBS1);

    // Put USART1 into 9 bit mode (set PB:UCSZ12,PC:UCSZ11,PC:UCSZ10) = (1,1,1):
    UCSR1B |= 1<<UCSZ12;
    UCSR1C |= 1<<UCSZ11 | 1<<UCSZ10;

    // Ignore Clock Polarity:
    //UCSR1C &= ~(1<<UCPOL1);

    // Disable multi-processor comm. mode (PA:MCPM1) = (0)
    //UCSR1A &= ~(1<<MPCM1);

    // Set UART1 baud rate for double data rate:
    UBRR1H = 0;
    UBRR1L = baud_lsb;
    UCSR1A |= 1<<U2X1;

    // Enable UART1:
    UCSR1B |= 1<<RXEN1;
    UCSR1B |= 1<<TXEN1;
} 

Bool8 Bus_Serial__can_receive() {
    Bool8 can_receive = (Bool8)0;
    if ((UCSR1A & (1<<RXC1)) != 0) {
	can_receive = (Bool8)1;
    }
    return can_receive;
}

Bool8 Bus_Serial__can_transmit() {
    Bool8 can_transmit = (Bool8)0;
    if ((UCSR1A & (1<<UDR1)) != 0) {
	can_transmit = (Bool8)1;
    }
    return can_transmit;
}

Frame Bus_Serial__frame_get() {
    // Wait for a frame (9-bits) to show up in the receive buffer:
    while (!((UCSR1A & (1<<RXC1)) != 0)) {
	// Do nothing:
    }

    //Serial.write("a:");
    //Serial.print(UCSR1A, HEX);
    //Serial.write(" ");

    // Deal with address bit:
    Frame frame = 0;
    if ((UCSR1B & (1<<RXB81)) != 0) {
	// Set the address bit:
	frame = 0x100;
    }

    // Read in the rest of the *frame*:
    frame |= (Frame)UDR1;
    return frame;
}

void Bus_Serial__frame_put(Frame frame) {
    // Wait for space in the transmit buffer:
    while (!((UCSR1A & (1<<UDRE1)) != 0)) {
        // do_nothing:
    }

    // Properly deal with adress bit:
    if ((frame & 0x100) == 0) {
	// No address bit:
	UCSR1B &= ~(1<<TXB81);
    } else {
	// Set the address bit:
	UCSR1B |= 1<<TXB81;
    }

    // Force *frame* out:
    UDR1 = (Byte)frame;
}

#endif // TEST == TEST_BUS_INPUT || TEST == TEST_BUS_ECHO

#if TEST == TEST_BUS_ECHO

// The *TEST_BUS_ECHO* test is used to ensure that two-way communication
// is occuring over the bus:

void loop() {
  if (Bus_Serial__can_receive())
  {
    Frame receive_frame = Bus_Serial__frame_get();
    Frame send_frame = receive_frame;
    Bus_Serial__frame_put(send_frame);
    Frame echo_frame = Bus_Serial__frame_get();
    
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
}

#endif // TEST == TEST_BUS_ECHO

#if TEST == TEST_BUS_INPUT

// The *TEST_BUS_INPUT* test is used to verify that the device
// has initialized UART1 to the correct baud rate, the CAN bus
// transceiver is not in standby mode, etc. :

void loop() {
  Serial.write("A:");
  Serial.print(UCSR1A, HEX);
  Serial.write(' ');
  Serial.write("B:");
  Serial.print(UCSR1B, HEX);
  Serial.write(' ');
  Serial.write("C:");
  Serial.print(UCSR1C, HEX);
  Serial.write("\r\n");

  // Get a *frame* from the bus:
  Frame frame = Bus_Serial__frame_get();

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
}

#endif // TEST == TEST_BUS_INPUT

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

  #if TEST == TEST_BUS_INPUT || TEST == TEST_BUS_ECHO
    // Initialize the BUS uart:
    Bus_Serial__initialize(BUS_BAUD_500KBPS);
  #endif // TEST == TEST_BUS_INPUT || TEST == TEST_BUS_ECHO
}

