// Generated file: only edit in designated areas!
#include <Bus_Bridge_Encoders_Sonar_Local.h>
#include <Bus_Slave.h>

// Put top level includes, typedef's here:
  //////// Edit begins here: TOP_LEVEL
  //////// Edit ends here: TOP_LEVEL

// Constructor
Bus_Bridge_Encoders_Sonar::Bus_Bridge_Encoders_Sonar(UByte address) {
  //////// Edit begins here: CONSTRUCTOR
  led_set(1);
  encoder1_set(0);
  encoder2_set(0);
  encoder1_reverse_set((Logical)0);
  encoder2_reverse_set((Logical)0);
  motor1_set(0);
  motor2_set(0);
  motor1_reverse_set((Logical)1);
  motor2_reverse_set((Logical)1);
  motors_encoders_swap_set((Logical)1);
  //////// Edit ends here: CONSTRUCTOR
}

// led_get: LED to control
Logical Bus_Bridge_Encoders_Sonar::led_get() {
  Logical result = 0;
  //////// Edit begins here: LED_GET
  result = _led;
  //////// Edit ends here: LED_GET
  return result;
}

// led_set: LED to control
void Bus_Bridge_Encoders_Sonar::led_set(Logical led) {
  //////// Edit begins here: LED_SET
  _led = led;
  digitalWrite(13, led);
  //////// Edit ends here: LED_SET
}

// encoder1_get: Encoder 1
Integer Bus_Bridge_Encoders_Sonar::encoder1_get() {
  Integer result = 0;
  //////// Edit begins here: ENCODER1_GET
  Integer encoder = _motors_encoders_swap ? _encoder2 :_encoder1;
  if (_encoder1_reverse) {
    encoder = -encoder;
  }
  result = encoder;
  //////// Edit ends here: ENCODER1_GET
  return result;
}

// encoder1_set: Encoder 1
void Bus_Bridge_Encoders_Sonar::encoder1_set(Integer encoder1) {
  //////// Edit begins here: ENCODER1_SET
  if (_encoder1_reverse) {
    encoder1 = -encoder1;
  }
  if (_motors_encoders_swap) {
    _encoder2 = encoder1;
  } else {
    _encoder1 = encoder1;
  }
  //////// Edit ends here: ENCODER1_SET
}

// encoder2_get: Encoder 2
Integer Bus_Bridge_Encoders_Sonar::encoder2_get() {
  Integer result = 0;
  //////// Edit begins here: ENCODER2_GET
  Integer encoder = _motors_encoders_swap ? _encoder1 :_encoder2;
  if (_encoder2_reverse) {
    encoder = -encoder;
  }
  result = encoder;
  //////// Edit ends here: ENCODER2_GET
  return result;
}

// encoder2_set: Encoder 2
void Bus_Bridge_Encoders_Sonar::encoder2_set(Integer encoder2) {
  //////// Edit begins here: ENCODER2_SET
  if (_encoder2_reverse) {
    encoder2 = -encoder2;
  }
  if (_motors_encoders_swap) {
    _encoder1 = encoder2;
  } else {
    _encoder2 = encoder2;
  }
  //////// Edit ends here: ENCODER2_SET
}

// motor1_get: Motor 1
Byte Bus_Bridge_Encoders_Sonar::motor1_get() {
  Byte result = 0;
  //////// Edit begins here: MOTOR1_GET
  result = _motor1;
  //////// Edit ends here: MOTOR1_GET
  return result;
}

// motor1_set: Motor 1
void Bus_Bridge_Encoders_Sonar::motor1_set(Byte motor1) {
  //////// Edit begins here: MOTOR1_SET

  // Save *motor1* for future reads:
  _motor1 = motor1;

  // Set the direction pins:
  Logical pin1 = (Logical)0;
  Logical pin2 = (Logical)1;
  if ((motor1 > 0 && _motor1_reverse) || (motor1 < 0 && !_motor1_reverse)) {
    // Reverse the motor direction:
    pin1 = (Logical)1;
    pin2 = (Logical)0;
  } else if ((motor1 & 0x7f) == 0) {
    // Put the motor into "brake" mode:
    pin2 = (Logical)0;
  } // else leave the motor in the forward direction:

  // Compute *pwm* (Pulse Width Modulation) value from *motor1*:
  if (motor1 < 0) {
    motor1 = -motor1;
  }
  UByte pwm = ((UByte)motor1 & 0x7f) << 1;
  if (motor1 == 0x80) {
    // A speed of 0x80 (== (Byte)-128), is full brake mode:
    pwm = 0xff;
  }
  if (_motors_encoders_swap) {
    digitalWrite(_direction_3a_pin, pin1);
    digitalWrite(_direction_4a_pin, pin2);
    analogWrite(_direction_enable_34_pin, pwm);
  } else {
    digitalWrite(_direction_1a_pin, pin1);
    digitalWrite(_direction_2a_pin, pin2);
    analogWrite(_direction_enable_12_pin, pwm);
  }
  //////// Edit ends here: MOTOR1_SET
}

// motor2_get: Motor 2
Byte Bus_Bridge_Encoders_Sonar::motor2_get() {
  Byte result = 0;
  //////// Edit begins here: MOTOR2_GET
  result = _motor2;
  //////// Edit ends here: MOTOR2_GET
  return result;
}

// motor2_set: Motor 2
void Bus_Bridge_Encoders_Sonar::motor2_set(Byte motor2) {
  //////// Edit begins here: MOTOR2_SET

  // Save *motor2* for future reads:
  _motor2 = motor2;

  // Set the direction pins:
  Logical pin1 = (Logical)0;
  Logical pin2 = (Logical)1;
  if ((motor2 > 0 && _motor2_reverse) || (motor2 < 0 && !_motor2_reverse)) {
    // Reverse the motor direction:
    pin1 = (Logical)1;
    pin2 = (Logical)0;
  } else if ((motor2 & 0x7f) == 0) {
    // Put the motor into "brake" mode:
    pin2 = (Logical)0;
  } // else leave the motor in the forward direction:

  // Compute *pwm* (Pulse Width Modulation) value from *motor2*:
  if (motor2 < 0) {
    motor2 = -motor2;
  }
  UByte pwm = ((UByte)motor2 & 0x7f) << 1;
  if (motor2 == 0x80) {
    // A speed of 0x80 (== (Byte)-128), is full brake mode:
    pwm = 0xff;
  }
  if (_motors_encoders_swap) {
    analogWrite(_direction_enable_12_pin, pwm);
    digitalWrite(_direction_1a_pin, pin1);
    digitalWrite(_direction_2a_pin, pin2);
  } else {
    analogWrite(_direction_enable_34_pin, pwm);
    digitalWrite(_direction_3a_pin, pin1);
    digitalWrite(_direction_4a_pin, pin2);
  }
  //////// Edit ends here: MOTOR2_SET
}

// motor1_reverse_get: Toggle motor direction.
Logical Bus_Bridge_Encoders_Sonar::motor1_reverse_get() {
  Logical result = 0;
  //////// Edit begins here: MOTOR1_REVERSE_GET
  result = _motor1_reverse;
  //////// Edit ends here: MOTOR1_REVERSE_GET
  return result;
}

// motor1_reverse_set: Toggle motor direction.
void Bus_Bridge_Encoders_Sonar::motor1_reverse_set(Logical motor1_reverse) {
  //////// Edit begins here: MOTOR1_REVERSE_SET
  _motor1_reverse = motor1_reverse;
  //////// Edit ends here: MOTOR1_REVERSE_SET
}

// motor2_reverse_get: Toggle motor direction.
Logical Bus_Bridge_Encoders_Sonar::motor2_reverse_get() {
  Logical result = 0;
  //////// Edit begins here: MOTOR2_REVERSE_GET
  result = _motor2_reverse;
  //////// Edit ends here: MOTOR2_REVERSE_GET
  return result;
}

// motor2_reverse_set: Toggle motor direction.
void Bus_Bridge_Encoders_Sonar::motor2_reverse_set(Logical motor2_reverse) {
  //////// Edit begins here: MOTOR2_REVERSE_SET
  _motor2_reverse = motor2_reverse;
  //////// Edit ends here: MOTOR2_REVERSE_SET
}

// encoder1_reverse_get: Toggle encoder direction.
Logical Bus_Bridge_Encoders_Sonar::encoder1_reverse_get() {
  Logical result = 0;
  //////// Edit begins here: ENCODER1_REVERSE_GET
  result = _encoder1_reverse;
  //////// Edit ends here: ENCODER1_REVERSE_GET
  return result;
}

// encoder1_reverse_set: Toggle encoder direction.
void Bus_Bridge_Encoders_Sonar::encoder1_reverse_set(Logical encoder1_reverse) {
  //////// Edit begins here: ENCODER1_REVERSE_SET
  _encoder1_reverse = encoder1_reverse;
  //////// Edit ends here: ENCODER1_REVERSE_SET
}

// encoder2_reverse_get: Toggle encoder direction.
Logical Bus_Bridge_Encoders_Sonar::encoder2_reverse_get() {
  Logical result = 0;
  //////// Edit begins here: ENCODER2_REVERSE_GET
  result = _encoder2_reverse;
  //////// Edit ends here: ENCODER2_REVERSE_GET
  return result;
}

// encoder2_reverse_set: Toggle encoder direction.
void Bus_Bridge_Encoders_Sonar::encoder2_reverse_set(Logical encoder2_reverse) {
  //////// Edit begins here: ENCODER2_REVERSE_SET
  _encoder2_reverse = encoder2_reverse;
  //////// Edit ends here: ENCODER2_REVERSE_SET
}

// motors_encoders_swap_get: Toggle encoder direction.
Logical Bus_Bridge_Encoders_Sonar::motors_encoders_swap_get() {
  Logical result = 0;
  //////// Edit begins here: MOTORS_ENCODERS_SWAP_GET
  result = _motors_encoders_swap;
  //////// Edit ends here: MOTORS_ENCODERS_SWAP_GET
  return result;
}

// motors_encoders_swap_set: Toggle encoder direction.
void Bus_Bridge_Encoders_Sonar::motors_encoders_swap_set(Logical motors_encoders_swap) {
  //////// Edit begins here: MOTORS_ENCODERS_SWAP_SET
  _motors_encoders_swap = motors_encoders_swap;
  //////// Edit ends here: MOTORS_ENCODERS_SWAP_SET
}

// encoders_reset: Reset both encoders to zero.
void Bus_Bridge_Encoders_Sonar::encoders_reset() {
  //////// Edit begins here: ENCODERS_RESET
  _encoder1 = 0;
  _encoder2 = 0;
  //////// Edit ends here: ENCODERS_RESET
}

// encoders_latch: Cause both encoder values to be latched.
void Bus_Bridge_Encoders_Sonar::encoders_latch() {
  //////// Edit begins here: ENCODERS_LATCH
  //////// Edit ends here: ENCODERS_LATCH
}

// command_process: Command process
UByte Bus_Bridge_Encoders_Sonar::command_process(Bus_Slave *bus_slave, UByte command, Logical execute_mode) {
  switch (command) {
    case 0: {
      // led_get: LED to control
      if (execute_mode) {
        Logical led = led_get();
        bus_slave->logical_put(led);
      }
      break;
    }
    case 1: {
      // led_set: LED to control
      Logical led = bus_slave->logical_get();
      if (execute_mode) {
        led_set(led);
      }
      break;
    }
    case 2: {
      // encoder1_get: Encoder 1
      if (execute_mode) {
        Integer encoder1 = encoder1_get();
        bus_slave->integer_put(encoder1);
      }
      break;
    }
    case 3: {
      // encoder1_set: Encoder 1
      Integer encoder1 = bus_slave->integer_get();
      if (execute_mode) {
        encoder1_set(encoder1);
      }
      break;
    }
    case 4: {
      // encoder2_get: Encoder 2
      if (execute_mode) {
        Integer encoder2 = encoder2_get();
        bus_slave->integer_put(encoder2);
      }
      break;
    }
    case 5: {
      // encoder2_set: Encoder 2
      Integer encoder2 = bus_slave->integer_get();
      if (execute_mode) {
        encoder2_set(encoder2);
      }
      break;
    }
    case 8: {
      // motor1_get: Motor 1
      if (execute_mode) {
        Byte motor1 = motor1_get();
        bus_slave->byte_put(motor1);
      }
      break;
    }
    case 9: {
      // motor1_set: Motor 1
      Byte motor1 = bus_slave->byte_get();
      if (execute_mode) {
        motor1_set(motor1);
      }
      break;
    }
    case 10: {
      // motor2_get: Motor 2
      if (execute_mode) {
        Byte motor2 = motor2_get();
        bus_slave->byte_put(motor2);
      }
      break;
    }
    case 11: {
      // motor2_set: Motor 2
      Byte motor2 = bus_slave->byte_get();
      if (execute_mode) {
        motor2_set(motor2);
      }
      break;
    }
    case 12: {
      // motor1_reverse_get: Toggle motor direction.
      if (execute_mode) {
        Logical motor1_reverse = motor1_reverse_get();
        bus_slave->logical_put(motor1_reverse);
      }
      break;
    }
    case 13: {
      // motor1_reverse_set: Toggle motor direction.
      Logical motor1_reverse = bus_slave->logical_get();
      if (execute_mode) {
        motor1_reverse_set(motor1_reverse);
      }
      break;
    }
    case 14: {
      // motor2_reverse_get: Toggle motor direction.
      if (execute_mode) {
        Logical motor2_reverse = motor2_reverse_get();
        bus_slave->logical_put(motor2_reverse);
      }
      break;
    }
    case 15: {
      // motor2_reverse_set: Toggle motor direction.
      Logical motor2_reverse = bus_slave->logical_get();
      if (execute_mode) {
        motor2_reverse_set(motor2_reverse);
      }
      break;
    }
    case 16: {
      // encoder1_reverse_get: Toggle encoder direction.
      if (execute_mode) {
        Logical encoder1_reverse = encoder1_reverse_get();
        bus_slave->logical_put(encoder1_reverse);
      }
      break;
    }
    case 17: {
      // encoder1_reverse_set: Toggle encoder direction.
      Logical encoder1_reverse = bus_slave->logical_get();
      if (execute_mode) {
        encoder1_reverse_set(encoder1_reverse);
      }
      break;
    }
    case 18: {
      // encoder2_reverse_get: Toggle encoder direction.
      if (execute_mode) {
        Logical encoder2_reverse = encoder2_reverse_get();
        bus_slave->logical_put(encoder2_reverse);
      }
      break;
    }
    case 19: {
      // encoder2_reverse_set: Toggle encoder direction.
      Logical encoder2_reverse = bus_slave->logical_get();
      if (execute_mode) {
        encoder2_reverse_set(encoder2_reverse);
      }
      break;
    }
    case 20: {
      // motors_encoders_swap_get: Toggle encoder direction.
      if (execute_mode) {
        Logical motors_encoders_swap = motors_encoders_swap_get();
        bus_slave->logical_put(motors_encoders_swap);
      }
      break;
    }
    case 21: {
      // motors_encoders_swap_set: Toggle encoder direction.
      Logical motors_encoders_swap = bus_slave->logical_get();
      if (execute_mode) {
        motors_encoders_swap_set(motors_encoders_swap);
      }
      break;
    }
    case 6: {
      // encoders_reset: Reset both encoders to zero.
      if (execute_mode) {
        encoders_reset();
      }
      break;
    }
    case 7: {
      // encoders_latch: Cause both encoder values to be latched.
      if (execute_mode) {
        encoders_latch();
      }
      break;
    }
  }
  return 0;
}
