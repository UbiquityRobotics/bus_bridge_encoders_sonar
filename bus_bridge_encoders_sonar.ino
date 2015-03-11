// Copyright (c) 2014-2015 by Wayne C. Gramlich.  All rights reserved.
// Some code came from the ros_arduino_bridge.

// The following tests are implemented by this code:
#define TEST_BUS_INPUT 1
#define TEST_BUS_OUTPUT 2
#define TEST_BUS_ECHO 3
#define TEST_BUS_COMMAND 4

// Set TEST to one of the possible tests:
#define TEST TEST_BUS_COMMAND

// Watch-out the code for SerialHardwared.cpp has been modified to
// notice this #define.  It causes the 8-ibt interrupt driver for
// UART1 to be disabled:
#define UART1_DISABLED

#include <Bus_Slave.h>
#include "Bus_Bridge_Encoders_Sonar_Local.h"

typedef struct {
  Double TargetTicksPerFrame;	// target speed in ticks per frame
  Integer Encoder;		// encoder count
  Integer PrevEnc;		// last encoder count

  // Using previous input (PrevInput) instead of PrevError to avoid derivative kick,
  // see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-derivative-kick/
  Short PrevInput;		// last input

  // Using integrated term (ITerm) instead of integrated error (Ierror),
  // to allow tuning changes,
  // see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/

  Short ITerm;			//integrated term

  Integer output;		// last motor setting
} Motor_Encoder;

Motor_Encoder leftPID, rightPID;

// The null object can be used for *debug_uart*:
NULL_UART null_uart;

// Debug *debug_uart* and *bus_uart* pointers:
#if defined(UDR1)
  UART *debug_uart = (UART *)&avr_uart0;
  UART *bus_uart = (UART *)&avr_uart1;
#else // defined(UDR1)
  UART *debug_uart = (UART *)&null_uart;
  UART *bus_uart = (UART *)&avr_uart0;
#endif // defined(UDR1)

// Define the pin names alphabetically:
static const UByte bus_standby_pin = A4;
static const UByte direction_1a_pin = 7;
static const UByte direction_enable_12_pin = 10;
static const UByte direction_2a_pin = 12;
static const UByte direction_3a_pin = 11;
static const UByte direction_enable_34_pin = 9;
static const UByte direction_enable_4a_pin = 8;
static const UByte encoder1_phase_a_pin = A0;
static const UByte encoder1_phase_b_pin = A1;
static const UByte encoder2_phase_a_pin = A2;
static const UByte encoder2_phase_b_pin = A3;
static const UByte encoders_enable_pin = 6;
static const UByte led_pin = 13;

// Encoder buffer:
#define BUFFER_POWER 8
#define BUFFER_SIZE (1 << BUFFER_POWER)
#define BUFFER_MASK (BUFFER_SIZE - 1)
static UByte encoder_buffer[BUFFER_SIZE];
static volatile UByte encoder_buffer_in = 0;
static volatile UByte encoder_buffer_out = 0;

// There are two encoders:
//
//    Encoder   Phase  Arduino Pin   PCInt
//    =======   =====  ===========   =======
//      1	  A        A0        PCInt8
//      1         B        A1        PCInt9
//      2         A        A2        PCInt10
//      2         B        A3        PCInt11

// This table is computed by bus_bridge_encoders_sonar.py
// Please read the comment in the Python code to understand
// the structure of the *state_transition_table* below:
static Byte state_transition_table[32] = {
    //            ccccc         sgg   // [xx]: SGG[gg] => sgg +/-
    (signed char)((0x00 << 3) | 0x0), // [00]: 000[00] => 000  0
    (signed char)((0x1f << 3) | 0x4), // [01]: 001[00] => 100 -1
    (signed char)((0x01 << 3) | 0x0), // [02]: 010[00] => 000 +1
    (signed char)((0x02 << 3) | 0x0), // [03]: 011[00] => 000 +2
    (signed char)((0x00 << 3) | 0x4), // [04]: 100[00] => 100  0
    (signed char)((0x1f << 3) | 0x4), // [05]: 101[00] => 100 -1
    (signed char)((0x01 << 3) | 0x0), // [06]: 110[00] => 000 +1
    (signed char)((0x1e << 3) | 0x4), // [07]: 111[00] => 100 -2
    (signed char)((0x01 << 3) | 0x1), // [08]: 1000[01] => 001 +1
    (signed char)((0x00 << 3) | 0x1), // [09]: 1001[01] => 001  0
    (signed char)((0x02 << 3) | 0x1), // [10]: 1010[01] => 001 +2
    (signed char)((0x1f << 3) | 0x5), // [11]: 1011[01] => 101 -1
    (signed char)((0x01 << 3) | 0x1), // [12]: 1100[01] => 001 +1
    (signed char)((0x00 << 3) | 0x5), // [13]: 1101[01] => 101  0
    (signed char)((0x1e << 3) | 0x5), // [14]: 1110[01] => 101 -2
    (signed char)((0x1f << 3) | 0x5), // [15]: 1111[01] => 101 -1
    (signed char)((0x1f << 3) | 0x6), // [16]: 10000[10] => 110 -1
    (signed char)((0x02 << 3) | 0x2), // [17]: 10001[10] => 010 +2
    (signed char)((0x00 << 3) | 0x2), // [18]: 10010[10] => 010  0
    (signed char)((0x01 << 3) | 0x2), // [19]: 10011[10] => 010 +1
    (signed char)((0x1f << 3) | 0x6), // [20]: 10100[10] => 110 -1
    (signed char)((0x1e << 3) | 0x6), // [21]: 10101[10] => 110 -2
    (signed char)((0x00 << 3) | 0x6), // [22]: 10110[10] => 110  0
    (signed char)((0x01 << 3) | 0x2), // [23]: 10111[10] => 010 +1
    (signed char)((0x02 << 3) | 0x3), // [24]: 11000[11] => 011 +2
    (signed char)((0x01 << 3) | 0x3), // [25]: 11001[11] => 011 +1
    (signed char)((0x1f << 3) | 0x7), // [26]: 11010[11] => 111 -1
    (signed char)((0x00 << 3) | 0x3), // [27]: 11011[11] => 011  0
    (signed char)((0x1e << 3) | 0x7), // [28]: 11100[11] => 111 -2
    (signed char)((0x01 << 3) | 0x3), // [29]: 11101[11] => 011 +1
    (signed char)((0x1f << 3) | 0x7), // [30]: 11110[11] => 111 -1
    (signed char)((0x00 << 3) | 0x7), // [31]: 11111[11] => 111  0
};

// PID constants:
static Short Kp = 20;	// PID Proportional Constant
static Short Kd = 12;	// PID Differential Constant
static Short Ki = 0;	// PID Integal Constant
static Short Ko = 50;	// PID common denOminator 
static Byte const MAX_PWM = 127;
static Logical is_moving = (Logical)0;

// *PCINT1_vect*() is the interrupt service routine for the
// pin change interrupts PCINT8/.../15.  The two encoders are
// attached to PCINT8/9/10/11, so these are the bits we want
// to capture.  This routine just stuffs the encoder bits
// into a buffer:

ISR(PCINT1_vect) {
  // Stuff the port C input bits into *buffer* and advance *buffer_in*:
  encoder_buffer[encoder_buffer_in] = PINC;
  encoder_buffer_in = (encoder_buffer_in + 1) & BUFFER_MASK;
  //Serial.print('.');
  //UDR0 = '.';
}

Bus_Slave bus_slave(bus_uart, debug_uart);
Bus_Bridge_Encoders_Sonar bus_bridge_encoders_sonar(33);

UByte command_process(Bus_Slave *bus_slave, 
 UByte command, Logical execute_mode) {
  return bus_bridge_encoders_sonar.command_process(bus_slave,
   command, execute_mode);
}

void pid_reset(Motor_Encoder *pid){
   pid->TargetTicksPerFrame = 0.0;
   // Leave *encoder* field alone:
   //pid->Encoder = 0;
   pid->PrevEnc = leftPID.Encoder;
   pid->output = 0;
   pid->PrevInput = 0;
   pid->ITerm = 0;
}

void do_pid(Motor_Encoder *pid) {
  Integer Perror;
  Integer output;
  Short input;

  //Perror = pid->TargetTicksPerFrame - (pid->Encoder - pid->PrevEnc);
  input = pid->Encoder - pid->PrevEnc;
  Perror = pid->TargetTicksPerFrame - input;

  // Avoid derivative kick and allow tuning changes, see:
  //
  //   http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-derivative-kick/
  //   http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/

  //output =
  // (Kp * Perror + Kd * (Perror - pid->PrevErr) + Ki * pid->Ierror) / Ko;
  // p->PrevErr = Perror;
  output = (Kp * Perror - Kd * (input - pid->PrevInput) + pid->ITerm) / Ko;
  pid->PrevEnc = pid->Encoder;

  output += pid->output;
  // Accumulate Integral error *or* Limit output.
  // Stop accumulating when output saturates
  if (output >= MAX_PWM)
    output = MAX_PWM;
  else if (output <= -MAX_PWM)
    output = -MAX_PWM;
  else
    // allow turning changes, see
    // http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/
    pid->ITerm += Ki * Perror;

  pid->output = output;
  pid->PrevInput = input;
}

void pid_update() {
  static Byte last_left_speed = 0x80;
  static Byte last_right_speed = 0x80;

  if (is_moving) {
    // Read the encoders:
    leftPID.Encoder = bus_bridge_encoders_sonar.encoder1_get();
    rightPID.Encoder = bus_bridge_encoders_sonar.encoder2_get();
  
    // Do the PID for each motor:
    //debug_uart->string_print((Text)"+");
    do_pid(&rightPID);
    do_pid(&leftPID);

    /* Set the motor speeds accordingly */
    //debug_uart->string_print((Text)" l=");
    //debug_uart->integer_print((UInteger)leftPID.output);
    //debug_uart->string_print((Text)" r=");
    //debug_uart->integer_print((UInteger)rightPID.output);
    //debug_uart->string_print((Text)"\r\n");

    Byte left_speed = (Byte)leftPID.output;
    Byte right_speed = (Byte)rightPID.output;

    if (left_speed != last_left_speed) {
	bus_bridge_encoders_sonar.motor1_set(left_speed);
	last_left_speed = left_speed;
    } 
    if (right_speed != last_right_speed) {
	bus_bridge_encoders_sonar.motor2_set(right_speed);
	last_right_speed = right_speed;
    }
    is_moving = (Logical)(left_speed != 0) || (right_speed != 0);

    //motor_speeds_set((Byte)leftPID.output, (Byte)rightPID.output);
  } else {
    //debug_uart->string_print((Text)"-");

    // If we're not moving there is nothing more to do:
    // Reset PIDs once, to prevent startup spikes, see
    //    http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-initialization/
    // PrevInput is considered a good proxy to detect
    // whether reset has already happened

    if (leftPID.PrevInput != 0 || rightPID.PrevInput != 0) {
	pid_reset(&leftPID);
	pid_reset(&rightPID);
    }
  }
}

void loop() {
  switch (TEST) {
    case TEST_BUS_COMMAND: {
      // Encoder positions and state:
      static UByte encoder1_state = 0;
      static UByte encoder2_state = 0;

      // Deal with any *bus* related activities:
      bus_slave.slave_mode(0x21, command_process);

      // Deal with encoder issues:
      if (encoder_buffer_in != encoder_buffer_out) {
	// Grab all 4 encoder bits from *encoder_buffer*:
	UByte encoder_bits = encoder_buffer[encoder_buffer_out];
	encoder_buffer_out = (encoder_buffer_out + 1) & BUFFER_MASK;

	// Just print out *encoder_bits*:
	//Serial.print(encoder_bits & 0x3);
	//Serial.print(":");
	//Serial.print((encoder_bits >> 2) & 0x3);
	//Serial.print("\n");

	// Process encoder1:
	// *encoder1_state* has 3 bits of state - '0000 0sss'.
	// We take encoder1 bits encoder from *encoder_bits* ('0000 00ee'),
	// shift them left by 3, and OR them with the *encoder1_state*:
	//
	//     encoder1_state:  0000 0sss
	//     encoder_bits<<3: 000e e000
	//  OR ==========================
	//     index            000e esss
	//
	UByte index = ((encoder_bits & 0x3) << 3) | encoder1_state;

	// Note that *state_transition* is signed.
	Byte state_transition = state_transition_table[index];
	bus_bridge_encoders_sonar._encoder1 += state_transition >> 3;
	encoder1_state = (unsigned char)(state_transition & 0x7);

	// Process encoder2:
	// Now we do the same for encoder2 whose bits are 2 bits over
	// ('0000 eexx'):
	index = ((encoder_bits & 0xc) << 1) | encoder2_state;
	state_transition = state_transition_table[index];
	bus_bridge_encoders_sonar._encoder2 += state_transition >> 3;
	encoder2_state = (unsigned char)(state_transition & 0x7);
      }
      break;
    }
    case TEST_BUS_ECHO: {
      // Wait for a *frame* to show up on *bus*:
      UShort frame = bus_slave.frame_get();

      // Set the *LED* to the least significant bit of *frame*:
      if ((frame & 1) == 0) {
        digitalWrite(led_pin, LOW);
      } else {
        digitalWrite(led_pin, HIGH);
      }

      // Print *frame* out to *debug_uart*:
      debug_uart->frame_put(frame & 0x7f);

      // Send out a CRLF when we get a '_':
      if (frame == (UShort)'_') {
        debug_uart->string_print((Character *)"\r\n");
      }

      // Now send *frame* back to the *bus*:
      bus_slave.frame_put(frame);

      // Get the *echo_frame* and check for a mismatch;
      UShort echo_frame = bus_slave.frame_get();
      if (frame != echo_frame) {
        debug_uart->string_print((Character *)"!");
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

      // Output *character* to the *debug_uart*:
      //debug_uart->frame_put((UShort)character);
      UDR0 = character;

      // Output any needed CRLF, and increment *character*:
      if (character == '_') {
	debug_uart->string_print((Character *)"\r\n");
	character = '@';
      } else {
	character += 1;
      }

      if ((character & 1) == 0) {
	digitalWrite(led_pin, LOW);
      } else {
	digitalWrite(led_pin, HIGH);
      }

      // Slow things down a little:
      delay(100);

      break;
    }
    case TEST_BUS_INPUT: {
      // This test will input *frame*'s from the *bus* and echo them
      // to *debug_uart*:

      // Get a *frame* from the bus:
      UShort frame = bus_slave.frame_get();

      // Print it out for debugging:
      debug_uart->frame_put(frame & 0x7f);

      // Tack on a CRLF when we get an '_':    
      if (frame == (UShort)'_') {
	debug_uart->string_print((Character *)"\r\n");
      }

      // Light the *LED* using the least significant bit of *frame*:
      if ((frame & 1) == 0) {
	digitalWrite(led_pin, LOW);
      } else {
	digitalWrite(led_pin, HIGH);
      }
      break;
    }
  }
}

void setup() {
  // The standard frequency for an AVR Arduino is 16MHz:
  static const UInteger frequency = (UInteger)16000000L;

  // Initalize the *debug_uart*:
  debug_uart->begin(frequency, 115200L, (Character *)"8N1");

  // Turn *LED* on:
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, HIGH);

  // Get *bus_uart* talking to the bus:
  bus_uart->begin(frequency, 500000L, (Character *)"9N1");

  // Force the standby pin on the CAN transeciever to low to force it
  // into active mode:
  pinMode(bus_standby_pin, OUTPUT);
  digitalWrite(bus_standby_pin, LOW);

  // Set up the H-Bridge pins:
  pinMode(direction_1a_pin, OUTPUT);
  pinMode(direction_enable_12_pin, OUTPUT);
  pinMode(direction_2a_pin, OUTPUT);
  pinMode(direction_3a_pin, OUTPUT);
  pinMode(direction_enable_34_pin, OUTPUT);
  pinMode(direction_enable_4a_pin, OUTPUT);  

  // Disable the motors:
  digitalWrite(direction_1a_pin, LOW);
  digitalWrite(direction_enable_12_pin, LOW);
  digitalWrite(direction_2a_pin, LOW);
  digitalWrite(direction_3a_pin, LOW);
  digitalWrite(direction_enable_34_pin, LOW);
  digitalWrite(direction_enable_4a_pin, LOW);

  // Set up the encoder pins:
  pinMode(encoder1_phase_a_pin, INPUT);
  pinMode(encoder1_phase_b_pin, INPUT);
  pinMode(encoder2_phase_a_pin, INPUT);
  pinMode(encoder2_phase_b_pin, INPUT);
  pinMode(encoders_enable_pin, OUTPUT);

  // Turn on encoder LED's:
  digitalWrite(encoders_enable_pin, HIGH);

  // Set up Interrupt on Pin Change interrupt vector.  The encoder
  // pins are attached to PCINT8/9/10/11, so we only need to set
  // PCMSK1 to '0000 1111':
  PCMSK1 = _BV(3) | _BV(2) | _BV(1) | _BV(0);

  // Now enable interrup on changes for PCINT8/.../15, by setting
  // PCICR to 1.  Thus, PCICR is set to 'xxxx x010' or '0000 0010':
  PCICR = _BV(1);

  // Enable global interrupts by setting the I bit (7th bit) in the
  // status register:
  //SREG |= _BV(7);

  // Enable/disable interrupts based on *TEST*:
  switch (TEST) {
    case TEST_BUS_OUTPUT:
      debug_uart = &avr_uart0;
      debug_uart->begin(frequency, 115200L, (Text)"8N1");
      debug_uart->string_print((Character *)"\r\nbbes_output:\r\n");
      debug_uart->interrupt_set((Logical)0);
      bus_uart->interrupt_set((Logical)0);
      break;
    case TEST_BUS_INPUT:
      debug_uart->string_print((Character *)"\r\nbbes_input:\r\n");
      debug_uart->interrupt_set((Logical)1);
      bus_uart->interrupt_set((Logical)1);
      break;
    case TEST_BUS_ECHO:
      debug_uart->string_print((Character *)"\r\nbbes_echo:\r\n");
      debug_uart->interrupt_set((Logical)0);
      bus_uart->interrupt_set((Logical)0);
      break;
    case TEST_BUS_COMMAND:
      debug_uart->string_print((Character *)"\r\nbbes_command:\r\n");
      debug_uart->interrupt_set((Logical)1);
      bus_uart->interrupt_set((Logical)1);
      break;
  }
}

