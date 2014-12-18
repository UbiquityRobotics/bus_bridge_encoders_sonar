// Copyright (c) 2014 by Wayne C. Gramlich.  All rights reserved.

#include "Arduino.h"
#include "Bus.h"

ISR(USART1_RX_vect)
{
  // This is the interrupt service routine that is called to when there
  // is a new frame (i.e. 9-bit value) from the Maker Bus USART.  This
  // Routine read the 9-bit value and either ignores for echo suppression,
  // or takes it and stuffs into the {_get_ring} buffer.

  // Read the data out; 9th bit is in UCSR1B, remaining 8-bits are in UDR1:
  UShort frame = 0;
  if ((UCSR1B & _BV(RXB81)) != 0) {
    frame = 0x100;
  }
  frame |= (UShort)UDR1;

  // If BUS_DEBUG is set to 1, set {log_frame} to {frame} OR'ed
  // with 0x1000 to indicate that this is a "get" frame:
  #if BUS_DEBUG != 0
    UShort log_frame = frame | 0x1000;
  #endif //BUS_DEBUG != 0

  UShort echo_suppress = 0;
  if ((echo_suppress & 0x8000) != 0) {
    // Mark that an echo suppress has occured with 0x200:
    #if BUS_DEBUG != 0
      log_frame |= 0x200;
    #endif // BUS_DEBUG != 0

    // Ignore frames until it matches {echo_suppress};
    if ((echo_suppress & 0x1ff) == frame) {
      // We have an echo match:
      bus._echo_suppress = 0;
    } // else keep on suppressing...
  } else {
    // Mark that we have a non-echo suppressed frame with 0x400:
    #if BUS_DEBUG != 0
      log_frame |= 0x400;
    #endif // BUS_DEBUG != 0

    // We need to insert {frame} into {_get_ring}.  If the {ring}
    // is full we drop {frame} on the floor:
    UByte get_head = bus._get_head;
    UByte new_get_head = (get_head + 1) & Bus::_get_ring_mask;

    // Is {_get_ring} full?
    if (new_get_head != bus._get_tail) {
      // No, {_get_ring} has some space for at least one more frame:

      // Stuff {frame} into {_get_ring}, and bump {_get_head} forward:
      bus._get_ring[get_head] = frame;
      bus._get_head = new_get_head;

      // If BUS_DEBUG is 1, mark the log frame with 0x800
      // to indicate that it mad it into {_get_ring}:
      #if BUS_DEBUG != 0
	log_frame |= 0x800;
      #endif // BUS_DEBUG != 0
    } // else {_get_ring} is full; drop {frame} on the floor:

    // For now we stay out of Multi-Processor Mode:
    UCSR1A &= ~_BV(MPCM1);
  }

  // We only stuff {log_frame} into {_log_buffer} is MAKE_BUS_DEBUG is 1:
  #if BUS_DEBUG != 0
    UByte log_total = bus._log_total;
    bus._log_buffer[log_total & BUS_LOG_MASK] = log_frame;
    bus._log_total = log_total + 1;
  #endif // BUS_DEBUG != 0
}

ISR(USART1_UDRE_vect)
{
  // This is the interrupt service routine that is invoked when the
  // transmit frame buffer is ready for another 9-bit frame.  The
  // frame is removed from {_put_buffer}.  If there are no frames
  // ready for transmission, the interrupt is disabled.

  // The transmit buffer is ready for a new frame.  Now check to
  // see if we have a frame to feed it:
  UByte put_tail = bus._put_tail;
  if (bus._put_head == put_tail) {
    // {_put_buffer} is empty, so disable interrupts:
    UCSR1B &= ~_BV(UDRIE1);
  } else {
    // {_put_buffer} is not empty; grab {frame} from {_put_buffer} and
    // update {_put_tail}:
    UShort frame = bus._put_ring[put_tail];
    bus._put_tail = (put_tail + 1) & Bus::_put_ring_mask;

    // Deal with 9th bit of {frame}.  Most of the time the 9th bit
    // is not set.  So we clear the 9th bit by default and then set
    // it if necssary:
    UCSR1B &= ~_BV(TXB81);
    if ((frame & 0x100) != 0) {
      // Set 9th bit:
      UCSR1B |= _BV(TXB81);
    }

    // Stuff the low order 8-bits into {UDR1} to send the 9-bit frame
    // on its way.
    UDR1 = (UByte)frame;

    // Only stuff {frame} into {_log_buffer} if {BUS_DEBUG} is 1:
    #if BUS_DEBUG != 0
      UByte log_total = bus._log_total;
      // Mark that the fame is a "put" by OR'ing in 0x2000:
      bus._log_buffer[log_total & BUS_LOG_MASK] = frame | 0x2000;
      bus._log_total = log_total + 1;
    #endif // BUS_DEBUG != 0

    // Supress echo:
    //bus._echo_suppress = frame | 0x8000;
  }
}

// *Bus_Buffer* routines:

Bus_Buffer::Bus_Buffer() {
  // Initialize the buffer indices and count.

  reset();
  //show('i');
}

void Bus_Buffer::reset() {
  _get_index = 0;
  _put_index = 0;
  _error_flags = 0;
}

UByte Bus_Buffer::check_sum() {
  // This routine will return the 4-bit checksum of the first {count}
  // bytes in {buffer}.

  UByte check_sum = 0;
  for (UByte index = 0; index < _put_index; index++) {
    check_sum += _ubytes[index];
    //trace_char('S');
    //trace_hex(ubyte);
  }
  return (check_sum + (check_sum >> 4)) & 0xf;
}

void Bus_Buffer::show(UByte tag) {
  trace_char('<');
  trace_char(tag);
  trace_char(':');
  trace_char('p');
  trace_hex(_put_index);
  trace_char('g');
  trace_hex(_get_index);
  trace_char('>');
}

UByte Bus_Buffer::ubyte_get() {
  // This routine will return the next byte from the buffer.

  return _ubytes[_get_index++ & _ubytes_mask];
}

void Bus_Buffer::ubyte_put(UByte ubyte) {
  // This routine will return the next byte from the buffer.

  _ubytes[_put_index++ & _ubytes_mask] = ubyte;
}

UShort Bus_Buffer::ushort_get() {
  // This routine will return the next short from the buffer.

  UByte high_byte = ubyte_get();
  UByte low_byte = ubyte_get();
  UShort ushort = (((UShort)high_byte) << 8) | ((UShort)low_byte);
  return ushort;
}

void Bus_Buffer::ushort_put(UShort ushort) {
  // This routine will return the next short from the buffer.

  ubyte_put((UByte)(ushort >> 8));
  ubyte_put((UByte)ushort);
}

// *Bus* routines:

Bus::Bus() {
  // We want to do the following to the USART:
  //  - Set transmit/receive rate to .5Mbps
  //  - Single rate transmission (Register A)
  //  - All interrupts disabled for now (Register B)
  //  - Transmit enable (Register B)
  //  - Receive enable (Register B)
  //  - Turn on Receiver interrupt enable flag (Register B)
  //  - 9-bits of data (Registers B and C)
  //  - Asynchronous mode (Register C)
  //  - No parity (Register C)
  //  - 1 Stop bit (Register C)
  //
  // There are a total of 5 registers to deal with.
  //   UBRRnL = USART Baud Rate Register (low byte)
  //   UBBRnH = USART Baud Rate Register (High byte)
  //   UCSRnA = USART Control status register A
  //   UCSRnB = USART Control status register B
  //   UCSRnC = USART Control status register C
  // They will be initialized in the order above.

  // Initialize USART1 baud rate to .5Mbps:
  //     f_cpu = 16000000L
  //     baud_rate = 500000L
  //     uubrr1 = f_cpu / (baud_rate * 16L) - 1
  //            = 16000000 / (500000 * 16) - 1
  //            = 16000000 / 8000000 - 1
  //            = 2 - 1
  //            = 1
  //FIXME: Should be 1 for .5Mbps rather 0 for 1Mbps
  UBRR1L = 1;
  UBRR1H = 0;

  // UCSR0A: USART Control and Status Register 0 A:
  //   rtef opdm: (0010 0000: Default):
  //      r: Receive complete
  //      t: Transmit complete
  //      e: data register Empty
  //      f: Frame error
  //      o: data Overrun
  //      p: Parity error
  //      d: Double transmission speed
  //	   m: Multi-processor communication mode
  // Only d and m can be set:  We want d=0 and m=0:
  //   rtef opdm = 0000 0000 = 0
  UCSR1A = 0;

  // UCSR0B: USART Control and Status Register 0 B:
  //   rtea bcde: (0000 0000: Default):
  //      r: Receive complete interrupt enable
  //      t: Transmit complete interrupt enable
  //      e: data register Empty complete interrupt enable
  //      a: (A) Transmitter enable
  //      b: (B) Receiver enable
  //      c: (C) size bit 2 (see register C):
  //      d: Receive data bit 8
  //	    e: Transmit data bit 8
  // All bits except d can be set.  We want 0001 1100 = 0x1c
  UCSR1B = _BV(TXEN1) | _BV(RXEN1) | _BV(UCSZ12); // = 0x1c

  // UCSR0C: USART Control and Status Register 0 C:
  //   mmpp szzp: (0000 0110):
  //      mm: USART Mode
  //          00 Asynchronous USART
  //          01 Synchronous USART
  //          10 reserved
  //          11 Master SPI
  //      pp: Parity mode
  //          00 Disabled
  //          01 reserved
  //          10 enabled, even parity
  //          11 enabled, odd parity
  //      s: Stop bit (0=1 stop bit, 1=2 stop bits)
  //      czz: Character size (include C bit from register B):
  //          000 5-bit
  //          001 6-bit
  //          010 7-bit
  //          011 8-bit
  //          10x reserved
  //          110 reserved
  //          111 9-bit
  //      p: Clock polarity (synchronous mode only)
  // All bits can set.  We want 0000 0110 = 6.
  UCSR1C = _BV(UCSZ11) | _BV(UCSZ10);	// = 6

  // Initialize some the private member values:
  _desired_address = (UShort)0;
  _current_address = (UShort)0xffff;

  _interrupt_mode = (Logical)0;
  _get_head = 0;
  _get_tail = 0;
  _put_head = 0;
  _put_tail = 0;
  _auto_flush = (Logical)1;

  UCSR1A &= ~_BV(MPCM1);
  //UCSR1B |= _BV(RXEN1);

  //UByte zilch = UDR1;
  //zilch += UDR1;
}

UByte Bus::command_ubyte_get(UByte address, UByte command) {
  command_begin(address, command, 0);
  UByte ubyte = ubyte_get();
  command_end();
  return ubyte;
}

void Bus::command_ubyte_put(UByte address, UByte command, UByte ubyte) {
  command_begin(address, command, sizeof(UByte));
  ubyte_put(ubyte);
  command_end();
}

void Bus::interrupts_enable() {
  _interrupt_mode = (Logical)1;
  UCSR1B |= _BV(RXCIE1) | _BV(UDRIE1);
}

void Bus::interrupts_disable() {
  _interrupt_mode = (Logical)0;
  UCSR1B &= ~(_BV(RXCIE1) | _BV(UDRIE1));
}

// The log_dump method is only provided if {BUS_DEBUG} is set to 1:
#if BUS_DEBUG != 0
void Bus::log_dump() {
  // This method will dump out the next batch of values from the
  // log buffer.

  // Enclose dump values in square brackets:
  Serial.write('\r');
  Serial.write('\n');
  Serial.write('[');

  // Iterate from {_log_dumped} to {_log_total}:
  UByte log_total = _log_total;
  UByte count = 0;
  UByte index;
  for (index = _log_dumped; index < log_total; index++) {
    // Fetch the next {frame}
    UShort frame =_log_buffer[index & BUS_LOG_MASK];

    // Prefix {frame} with 'g' for get and 'p' for put:
    if ((frame & 0x1000) != 0) {
      Serial.print('g');
    } else {
      Serial.print('p');
    }

    // Output the 9-bit frame value:
    Serial.print(frame & 0x1ff, HEX); 

    // Output the frame flags:
    Serial.write(':');
    Serial.print((frame >> 8) & 0xe, HEX);

    // Separate the value with a space:
    Serial.write(' ');

    // To prevent line wrapping, insert a new-line every 8th value:
    if ((count & 7) == 7) {
      Serial.write('\r');
      Serial.write('\n');
     }
     count += 1;
  }

  // Remember that we have dumped up to {log_total}:
  _log_dumped = log_total;

  // Close off the square brackets:
  Serial.write(']');
  Serial.write('\r');
  Serial.write('\n');
}
#endif // BUS_DEBUG != 0

// *Bus::command_begin*() will queue up *command* to be sent to the
// module at *address*.  The number of bytes to be sent after *command*
// is *put_bytes*.  If there is inadequate space in the send buffer
// for *command* and the following *put_bytes* of data, any previous
// command(s) are flushed first.

void Bus::command_begin(UByte address, UByte command, UByte put_bytes) {
    // For debugging, commands get enclosed in '{' ... '}':
    trace_char('{');

    // Force a flush if the address is
    UShort full_address = (UShort)address | 0x100;
    if (_desired_address != full_address) {
	flush();
	_desired_address = full_address;
    }

    // If command will not fit into *remaining* bytes, force a flush:
    UByte remaining = _maximum_request_size - _put_buffer._put_index;
    if (1 + put_bytes > remaining) {
	flush();
    }

    // Stuff the *command* byte into to the *put_buffer*:
    ubyte_put(command);
}

void Bus::command_end() {
  // This command indicates that the current command is done.

  // When *auto_flush* is enabled, we force a flush at the
  // end of each command:
  if (_auto_flush) {
    flush();
  }

  // Close off '}' for debugging:
  trace_char('}');
  trace_char('\r');
  trace_char('\n');
}

UShort Bus::ushort_get() {
  //...

  UByte high_byte = ubyte_get();
  UByte low_byte = ubyte_get();
  UShort ushort = (((UShort)high_byte) << 8) | ((UShort)low_byte);
  return ushort;
}

void Bus::ushort_put(UShort ushort) {
  // ...

  ubyte_put((UByte)(ushort >> 8));
  ubyte_put((UByte)ushort);
}

UByte Bus::ubyte_get() {
  // ...

  return (UByte)_get_buffer.ubyte_get();
}

void Bus::ubyte_put(UByte ubyte) {
  // ...

  _put_buffer.ubyte_put(ubyte);
}

// Flush current buffer and get any response back:
Logical Bus::flush() {
  trace_char('!');
  Logical error = (Logical)0;

  // See if there is anything to flush:
  UByte request_size = _put_buffer._put_index;
  if (request_size > 0) {

    // If {BUS_DEBUG} is set 1, dump out the frame log:
    #if BUS_DEBUG != 0
	log_dump();
    #endif // BUS_DEBUG != 0

    // For now force the *desired_address* out:
   _current_address = (UShort)0xffff;

    // Keep sending out an address until we succeed:
    while (_current_address != _desired_address) {
      // Make sure that there is nothing left in receive queue:
      while (can_receive()) {
	(void)frame_get();
      }

      // Send the *desired_address*:
      frame_put(_desired_address);
      UShort address_echo = frame_get();
      if (_desired_address == address_echo) {
	// Deal with addresses that are in the lower half:
	if ((_desired_address & 0x80) == 0) {
	  // Wait for an acknowledgement byte:
	  UShort acknowledge_frame = frame_get();
	  if (acknowledge_frame == 0) {
	    // Success: we have set the *current_address*:
	    _current_address = _desired_address;
	  } else {
	    trace_char('?');
	  }
	} else {
	  // Success: we have set the *current_address*:
	  _current_address = _desired_address;
	}
      } else {
	trace_char('@');
      }
    }
    // assert (_current_address == _desired_address);

    // Send the *request_header*:
    UByte request_size = _put_buffer._put_index;
    UByte request_check_sum = _put_buffer.check_sum();
    UByte request_ubyte = (request_size << 4) | request_check_sum;
    UShort request_frame = (UShort)request_ubyte;    
    frame_put(request_frame);
    UByte request_echo = frame_get();
    if (request_frame != request_echo) {
      trace_char('#');
    }

    // Send the request data followed by reseting the buffer:
    UByte index;
    for (index = 0; index < request_size; index++) {
      UShort ubyte_frame = (UShort)_put_buffer.ubyte_get();
      frame_put(ubyte_frame);
      UShort ubyte_echo = frame_get();
      if (ubyte_frame != ubyte_echo) {
	trace_char('$');
      }
    }
    _put_buffer.reset();

    // Wait for the response packet header:
    UByte response_header = (UByte)frame_get();

    // Process *response_header*:
    UByte response_length = response_header >> 4;
    if (response_length == 0) {
      // A *response_length* of 0 indicates an error:
      error = (Logical)1;
    } else {
      // Now slurp in the rest of response packet:
      _get_buffer.reset();
      for (index = 0; index < response_length; index++) {
      UByte ubyte = (UByte)frame_get();
	_get_buffer.ubyte_put(ubyte);
      }

      // Check for a check sum error:
      UByte response_check_sum = response_header & 0xf;
      if (_get_buffer.check_sum() != response_check_sum) {
	error = (Logical)1;
      }
    }

  }
  trace_char('!');
  return error;
}

Logical Bus::can_receive() {
  Logical result = (Logical)0;
  if (_interrupt_mode) {
    result = (Logical)(bus._get_tail != bus._get_head);
  } else {
    result = (Logical)((UCSR1A & _BV(RXC1)) != 0);
  }
  return result;
}


Logical Bus::can_transmit() {
  Logical result = (Logical)0;
  if (_interrupt_mode) {
    UByte next_put_head = (_put_head + 1) & _put_ring_mask;
    result = (Logical)(next_put_head != _put_tail);
  } else {
    result = (Logical)((UCSR1A & _BV(UDRE1)) != 0);
  }
  return result;
}

UShort Bus::frame_get() {
  // Wait for a 9-bit frame to arrive and return it:

  trace_char('g');
  UCSR1B |= _BV(RXEN1);
  UShort frame = 0;

  // Set to 1 to use interrupt buffers; 0 for direct UART access:
  if (_interrupt_mode) {
    // When tail is equal to head, there are no frames in ring buffer:
    UByte get_tail = bus._get_tail;
    while (bus._get_tail == bus._get_head) {
      // Wait for a frame to show up.
    }

    // Read the {frame} and advance the tail by one:
    frame = bus._get_ring[get_tail];
    bus._get_tail = (get_tail + 1) & _get_ring_mask;
  } else {
    while (!(UCSR1A & _BV(RXC1))) {
      // Nothing yet, keep waiting:
    }
    if ((UCSR1B & _BV(RXB81)) != 0) {
      frame |= 0x100;
    }
    frame |= (UShort)UDR1;
  }
  trace_hex(frame);
  return frame;
}

void Bus::frame_put(UShort frame) {
  // This routine will output the low order 9-bits of {frame} to {self}.
  // The echo due to the fact the bus is half-duplex is automatically
  // read and ignored.

  trace_char('p');
  trace_hex(frame);

  // Set to 1 to use interrupt buffers; 0 for direct UART access:
  if (_interrupt_mode) {
    UByte put_head = bus._put_head;
    UByte new_put_head = (put_head + 1) & _put_ring_mask;
    while (new_put_head == bus._put_tail) {
      // Wait for space to show up in put ring buffer:
    }

    bus._put_ring[put_head] = frame;
    bus._put_head = new_put_head;

    UCSR1B |= _BV(UDRIE1);
  } else {
    // Wait until UART can take another character to output:
    while (!(UCSR1A & _BV(UDRE1))) {
      // UART is still busy, keep waiting:
    }
  
    // Set 9th bit:
    if ((frame & 0x100) != 0) {
      UCSR1B |= _BV(TXB81);
    } else {
      UCSR1B &= ~_BV(TXB81);
    }

    // UART is ready, output the character:
    UDR1 = (UByte)frame;
  }
}

// This routine will perform all the operations to respond to
// commands sent to *address*.  *command_process* is a routine that
// is called to process each command in the received request packet.
// If *execute_mode*, the command is to be executed; otherwise the
// command is only parsed for correctness.  The return value
// from *command_process* is zero for success and non-zero for
// for failure.

void Bus::slave_mode(UByte address,
 UByte (*command_process)(Bus *bus, UByte command, Logical execute_mode)) {
  Logical selected = (Logical)0;
  UByte request_size = 0;
  UByte request_check_sum = 0;
  UByte selected_address = 0xff;
  trace_char('[');
  while (1) {
    // Fetch the next frame from the UART:
    UShort frame = frame_get();

    // Dispatch on 9th bit:
    if ((frame & 0x100) != 0) {
      // We have an address frame:
      //trace_char('J');
      selected_address = (UByte)frame;
      selected = (Logical)(selected_address == address);
      if (selected) {
	// We have been selected:
	//trace_char('K');
	selected = (Logical)1;
	if ((address & 0x80) == 0) {
	  // We need to send an acknowledge
	  //trace_char('L');
	  frame_put(0x0);
	  //trace_char('M');
	  UShort acknowledge_echo = frame_get();
	  if (acknowledge_echo != 0) {
	    trace_char('!');
	  }
	}
	_get_buffer.reset();
	_put_buffer.reset();
      }
      trace_char('N');

      // We are starting over:
      request_size = 0;
    } else if (selected) {
      // We have a data frame:
      UByte data = (UByte)frame;

      if (request_size == 0) {
	// Process header request:
	//trace_char('m');
	request_size = (data >> 4) & 0xf;
	request_check_sum = data & 0xf;

	_get_buffer.reset();
      } else {
	// Take {data} byte and stuff it onto end of {_get_buffer}:
	_get_buffer.ubyte_put(data);

	// Do we have a complete request?
	if (_get_buffer._put_index >= request_size) {
	  // Yes, we have a complete request:

	  // Check that *request_check_sum* matches *check_sum*:
	  UByte check_sum = _get_buffer.check_sum();
	  //trace_char('o');
	  //trace_hex(check_sum);
	  if (check_sum == request_check_sum) {
	    // The check sums match; now iterate over all the
	    // commands and make sure that they parse correctly.
	    // We pass over the request bytes twice.  The first
	    // time ({pass} == 0), we just make sure that the
	    // commands and associated arguments "make sense".
	    // The second time ({pass} == 1), we actually perform
	    // the commands:
	    UByte flags;
	    UByte pass;

	    // Pass over request bytes two times:
	    for (pass = 0; pass < 2; pass++) {
	      // Now iterate over all the command sequences
	      // in {_get_buffer}:
	      flags = 0;
	      _get_buffer._get_index = 0;
	      while (_get_buffer._get_index < request_size) {
		UByte command = ubyte_get();
		flags |= command_process(this, command, (Logical)pass);
	      }
	      //trace_char('r');

	      // Make sure we detect errors from trying to
	      // read too many bytes from request:
	      flags |= _get_buffer._error_flags;

	      // If there are any errors, we generate an error
	      // response and do not perform the second pass:
	      _get_buffer._error_flags = 0;
	      if (flags != 0) {
		trace_char('q');
		// We have a parse error:
		break;
	      }
	      //trace_char('s');
	    }

	    //trace_char('v');
	    // Did we have any errors:
	    if (flags == 0) {
	      // Time to pump out a response packet:

	      // Compute {response_header} and output it:
	      UByte response_size = _put_buffer._put_index;
	      UByte response_check_sum = _put_buffer.check_sum();
	      UByte header_byte = (response_size << 4) | response_check_sum;
	      UShort header_frame = (UShort)(header_byte);
	      frame_put(header_frame);
	      UShort header_echo = frame_get();
	      if (header_frame != header_echo) {
		trace_char('/');
	      }
	      //trace_char('y');

	      for (UByte index = 0; index < response_size; index++) {
		UShort ubyte_frame = (UShort)_put_buffer.ubyte_get();
		frame_put(ubyte_frame);
		UShort ubyte_echo = frame_get();
		if (ubyte_frame != ubyte_echo) {
		  trace_char(';');
		}
	      }
	      _put_buffer.reset();
	    } else {
	      // We had at least one error; kick out an error:
	      //trace_char('z');
	      trace_hex(flags);
	      frame_put((UShort)0x03);
	    }
	  } else {
	    // The checksums do match; respond with an error byte:
	    //trace_char('w');
	    //trace_hex(request_checksum);
	    //trace_char('x');
	    //trace_hex(checksum);
	    frame_put((UShort)0x01);
	  }

	  request_size = 0;
	  trace_char(']');
	  trace_char('\r');
	  trace_char('\n');
	  trace_char('[');
	}
      }
    }
  }
}

void Bus_Module::bind(Bus *bus, UByte address)
{
  _bus = bus;
  _address = address;
}


// Send the contents of *Bus_Serial__put_buffer* to the bus:
Logical Bus::put_buffer_send() {
  Logical error = (Logical)0;

  // Compute the 4-bit *check_sum*:
  UByte check_sum = _put_buffer.check_sum();

  // Send *count_check_sum*:
  UByte size = _put_buffer._put_index & 0xf;

  UByte header = (size << 4) | check_sum;

  Serial.write("P");
  Serial.print(header, HEX);

  bus.frame_put((UShort)header);
  UShort header_echo = bus.frame_get();
  if (header == header_echo) {
    // Send the buffer:
    for (Byte index = 0; index < size; index++) {
      UShort put_frame = (UShort)_put_buffer._ubytes[index];

      Serial.write("P");
      Serial.print(put_frame, HEX);

      bus.frame_put(put_frame);
      UShort put_frame_echo = bus.frame_get();
      if (put_frame != put_frame_echo) {
        Serial.write('&');
        error = (Logical)1;
        break;
      }
    }
  } else {
    // *count_check_sum_echo* did not match *count_check_sum*:
    error = (Logical)1;
  }

  // We mark *Bus_Serial__put_buffer* as sent:
  _put_buffer.reset();
  return error;
}

