#define UART1_DISABLED

// #includes:
//#include "MB7.h"
#include "Bus.h"
#include <Bus_Bridge_Encoders_Sonar.h>

// Object variables:
Bus bus;
Bus_Bridge_Encoders_Sonar bus_bridge_encoders_sonar(133);

// Forward declaration of command_process():
UByte command_process(Bus *bus, UByte command, Logical execute_mode);

void setup() {
//  Serial.begin(9600);
//  Serial.print("\nBus_Bridge_Encoders_Sonar:\n");
}


void loop() {
  bus.slave_mode(133, command_process);
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
