#define UART1_DISABLED

#define BUS_BAUD_500KBPS 3 // Fosc=16MHz & U2X1=1
#define LED 13
#define BUS_STANDBY A4


// typedef's go here:
typedef unsigned char Bool8;
typedef unsigned char Byte;
typedef unsigned short Frame;
typedef struct Frame_Buffer__Struct *Frame_Buffer;
typedef unsigned short UInt16;

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

Bool8 Bus_Serial__can_receive(void) {
    Bool8 can_receive = (Bool8)0;
    if ((UCSR1A & (1<<RXC1)) != 0) {
	can_receive = (Bool8)1;
    }
    return can_receive;
}

Bool8 Bus_Serial__can_transmit(void) {
    Bool8 can_transmit = (Bool8)0;
    if ((UCSR1A & (1<<UDRE1)) != 0) {
	can_transmit = (Bool8)1;
    }
    return can_transmit;
}

Frame Bus_Serial__frame_get(void) {
    // Wait for a frame (9-bits) to show up in the receive buffer:
    while (!((UCSR1A & (1<<RXC1)) != 0)) {
	// Do nothing:
    }

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

// #includes:
//#include "MB7.h"
//#include "Bus.h"
//#include <Bus_Bridge_Encoders_Sonar.h>

//ISR(PCINT0_vect) {
//    
//}

// Object variables:
//Bus bus;
//Bus_Bridge_Encoders_Sonar bus_bridge_encoders_sonar(133);

// Forward declaration of command_process():
//UByte command_process(Bus *bus, UByte command, Logical execute_mode);

void setup() {
  // Initalize the debugging port:
  Serial.begin(115200);
  Serial.print("\r\nbus_bridge_encoders_sonar:\r\n");

  // Initialize the BUS uart:
  Bus_Serial__initialize(BUS_BAUD_500KBPS);

  // Turn *LED* on:
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // Force the standby pin on the CAN transeciever to low to force it
  // into active mode:
  pinMode(BUS_STANDBY, OUTPUT);
  digitalWrite(BUS_STANDBY, LOW);
}

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
  //bus.slave_mode(133, command_process);
}

//UByte command_process(Bus *bus, UByte command, Logical execute_mode) {
//  switch (command) {
//    // Bus_Bridge_Encoders_Sonar
//    case 0: {
//      // led_get:
//      if (execute_mode) {
//	Logical led = bus_bridge_encoders_sonar.led_get();
//        bus->logical_put(led);
//      }
//      break;
//    }
//    case 1: {
//      // led_set:
//      Logical led = bus->logical_get();
//      if (execute_mode) {
//        bus_bridge_encoders_sonar.led_set(led);
//      }
//      break;
//    }
//  }
//  return 0;
//}

//Bus_Bridge_Encoders_Sonar::Bus_Bridge_Encoders_Sonar(UByte address) {
//    _address = address;
//    _led = (Logical)0;
//}

//Logical Bus_Bridge_Encoders_Sonar::led_get() {
//    return _led;
//}
//
//void Bus_Bridge_Encoders_Sonar::led_set(Logical led) {
//    _led = led;
//    digitalWrite(13, led);
//}
