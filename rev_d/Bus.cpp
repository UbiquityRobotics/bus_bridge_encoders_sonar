
#include "Arduino.h"
#include "Bus.h"

void serialEvent1() __attribute__((weak));
void serialEvent1() {}

//void serialEventRun(void)
//{
//    if (Serial1.available()) {
//	serialEvent1();
//    }
//}

SIGNAL(USART1_RX_vect)
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

    //if ((frame & 0x100) != 0) {
    //	bus._get_tail = 0;
    //	bus._get_head = 0;
    //}

    UShort echo_suppress = bus._echo_suppress;
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
	UByte new_get_head = (get_head + 1) & GET_RING_MASK;

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

	// We should only get an address frame in slave mode:
	//if (frame == bus._slave_address) {
	//    // {address} does match, receive address and data bytes:
	//    UCSR1A &= ~_BV(MPCM1);
	//} else {
	//    // {address} does not match, only deal with address bytes:
	//    UCSR1A |= _BV(MPCM1);
	//} // else we should not get an address byte in master mode:
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
	bus._put_tail = (put_tail + 1) % PUT_RING_MASK;

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
	    bus._log_buffer[log_total & BUS_LOG_MASK] =
	      frame | 0x2000;
	    bus._log_total = log_total + 1;
	#endif // BUS_DEBUG != 0

	// Supress echo:
	bus._echo_suppress = frame | 0x8000;
    }
}

/*
 */

Bus_Buffer::Bus_Buffer() {
    // Initialize the buffer indices and count.

    reset();
    //show('i');
}

void Bus_Buffer::reset() {
    _count = 0;
    _get_index = 0;
    _put_index = 0;
    _error_flags = 0;
}

UByte Bus_Buffer::checksum(UByte checksum_count) {
    // This routine will return the 4-bit checksum of the first {count}
    // bytes in {buffer}.

    UByte checksum = 0;
    UByte index = _get_index;
    while (checksum_count != 0) {
	UByte ubyte = _ubytes[index++ & Bus_Buffer__mask];
	//trace_char('S');
	//trace_hex(ubyte);
	checksum += ubyte;
	checksum_count--;
    }
    return (checksum + (checksum >> 4)) & 0xf;
}

void Bus_Buffer::show(UByte tag) {
    trace_char('<');
    trace_char(tag);
    trace_char(':');
    trace_char('c');
    trace_hex(_count);
    trace_char('p');
    trace_hex(_put_index);
    trace_char('g');
    trace_hex(_get_index);
    trace_char('>');
}

UByte Bus_Buffer::ubyte_get() {
    // This routine will return the next byte from the buffer.

    _count--;
    return _ubytes[_get_index++ & Bus_Buffer__mask];
}

void Bus_Buffer::ubyte_put(UByte ubyte) {
    // This routine will return the next byte from the buffer.

    _count++;
    _ubytes[_put_index++ & Bus_Buffer__mask] = ubyte;
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
    // All bits except d can be set.  We want 1001 1100 = 0x1c
    UCSR1B = _BV(RXCIE1) | _BV(TXEN1) | _BV(RXEN1) | _BV(UCSZ12); // = 0x9c

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
    //_slave_address = 0xffff;
    _address = 0;
    _last_address = 0xff;

    _get_head = 0;
    _get_tail = 0;
    _put_head = 0;
    _put_tail = 0;
    _auto_flush = 1;

    UCSR1A &= ~_BV(MPCM1);
    UCSR1B |= _BV(RXEN1);

    UByte zilch = UDR1;
    zilch += UDR1;
}

// The log_dump method is only provided if {BUS_DEBUG} is set to 1:
#if BUS_DEBUG != 0
void Bus::log_dump() {
    // This method will dump out the next batch of values from the
    // log buffer.

    // Enclose dump values in square brackets:
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
	    Serial.write('\n');
	}
	count += 1;
    }

    // Remember that we have dumped up to {log_total}:
    _log_dumped = log_total;

    // Close off the square brackets:
    Serial.write(']');
    Serial.write('\n');
}
#endif // BUS_DEBUG != 0

void Bus::command_begin(UByte address, UByte command) {
    // This routine will start a new command for the module at {address}.
    // The first byte of the command is {command}.

    trace_char('<');
    if (address != _address) {
	// Flush any pending commands:
	flush();

	// Send out the new {address}:
	frame_put((UShort)address | 0x100);
	_address = address;

	if ((address & 0x80) == 0) {
	    (void)frame_get();
	}
    }

    _commands_length = _put_buffer._count;

    ubyte_put(command);
}

void Bus::command_end() {
    // This command indicates that the current command is done.

    if (_auto_flush) {
	flush();
    }
    trace_char('>');
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

    //flush();
    return (UByte)_get_buffer.ubyte_get();
}

void Bus::ubyte_put(UByte ubyte) {
    // ...

    _put_buffer.ubyte_put(ubyte);
}

void Bus::flush() {
    // Flush current buffer:

    // If {BUS_DEBUG} is set 1, dump out the frame log:
    #if BUS_DEBUG != 0
	log_dump();
    #endif // BUS_DEBUG != 0
    trace_char('!');

    // It may take a couple of packets request/repond pairs to clear out buffer:
    UByte commands_length = _commands_length;
    UByte put_buffer_count = _put_buffer._count;
    while (put_buffer_count != 0) {
	// The maximum request packet length is 15.  If we are bigger than
	// that we use current {commands_length}; otherwise we can output
	// the entier buffer in one packet:
	if (put_buffer_count <= 15) {
	    commands_length = put_buffer_count;
	}

	// Send the request header frame:
	UByte checksum = _put_buffer.checksum(commands_length);
	UByte request_header = (commands_length << 4) | checksum;
	frame_put((UShort)request_header);

	// Send the request packet data:
	UByte index;
	for (index = 0; index < commands_length; index++) {
	    UByte ubyte = _put_buffer.ubyte_get();
	    frame_put((UShort)ubyte);
	}

	// Upate the buffer count to indicate that we have shipped off
	// {commands_length} bytes:
	put_buffer_count -= commands_length;
	_put_buffer._count = put_buffer_count;

	// Wait for the response packet header:
	UByte response_header = (UByte)frame_get();
	UByte response_length = response_header >> 4;

	// Now slurp in the response packet:
	checksum = 0;
	for (index = 0; index < response_length; index++) {
	    UByte ubyte = (UByte)frame_get();
	    checksum += ubyte;
	    _get_buffer.ubyte_put(ubyte);
	}

	// FIXME: Check checksum here!!!
    }
    // All commands have been sent off:
    _commands_length = 0;
}

UShort Bus::frame_get() {
    // Wait for a 9-bit frame to arrive and return it:

    trace_char('g');
    UCSR1B |= _BV(RXEN1);
    UShort frame = 0;

    // Set to 1 to use interrupt buffers; 0 for direct UART access:
    if (1) {
	// When tail is equal to head, there are no frames in ring buffer:
	UByte get_tail = bus._get_tail;
	while (bus._get_tail == bus._get_head) {
	  // Wait for a frame to show up.
	}

	// Read the {frame} and advance the tail by one:
	frame = bus._get_ring[get_tail];
	bus._get_tail = (get_tail + 1) & GET_RING_MASK;
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
    if (1) {
	UByte put_head = bus._put_head;
	UByte new_put_head = (put_head + 1) % PUT_RING_MASK;
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

    // Clear the echo:
    //(void)frame_get();
}

void Bus::slave_mode(UByte address,
  UByte (*command_process)(Bus *bus,
  UByte command, Logical execute_mode)) {
    // This routine will perform all the operations to respond to
    // commands sent to {address}.  {command_process} is a routine that
    // is called to process each command in the received request packet.
    // If {execute_mode}, the command is to be executed; otherwise the
    // command is only parsed for correctness.  The return value
    // from {command_process} is zero for success and non-zero for
    // for failure.

    Logical selected = (Logical)0;
    UByte request_length = 0;
    UByte request_checksum = 0;
    UByte request_index = 0;
    UByte selected_address = 0xff;
    trace_char('[');
    while (1) {
	// Fetch the next frame from the UART:
	UShort frame = frame_get();

	// Dispatch on 9th bit:
	if ((frame & 0x100) != 0) {
	    // We have an address frame:
	    selected_address = (UByte)frame;
	    selected = (Logical)(selected_address == address);
	    if (selected) {
		// We have been selected:
		selected = (Logical)1;
		if ((address & 0x80) == 0) {
		    // We need to send an acknowledge
		    frame_put(0xa5);
		}
		_get_buffer.reset();
		_put_buffer.reset();
	    }

	    // We are starting over:
	    request_length = 0;
	} else if (selected) {
	    // We have a data frame:
	    UByte data = (UByte)frame;

	    if (request_length == 0) {
		// Process header request:
		//trace_char('m');
		request_length = (data >> 4) & 0xf;
		request_checksum = data & 0xf;

		// Keep track of where the request starts in {_get_buffer}.
		// This is needed as part of the "parse check" code:
		request_index = _get_buffer._get_index;
	    } else {
		// Take {data} byte and stuff it onto end of {_get_buffer}:
		_get_buffer.ubyte_put(data);

		// Do we have a complete request?
		if (_get_buffer._count == request_length) {
		    // Yes, we have a complete request:

		    // Compute the request checksum:
		    UByte checksum = _get_buffer.checksum(request_length);
		    //trace_char('o');
		    //trace_hex(checksum);

		    // Check that checksum matches checksum from header:
		    if (checksum == request_checksum) {
			// The checksums match; now iterate over all the
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
			    //trace_char('P');
			    if (pass == 1) {
				// For second pass, reset {_get_buffer} to
				// ensure we go over the same bytes again:
				_get_buffer._get_index = request_index;
				_get_buffer._count = request_length;
			    }

			    // Now iterate over all the command sequences
			    // in {_get_buffer}:
			    flags = 0;
			    while (_get_buffer._count != 0) {
				UByte command = ubyte_get();
				flags |=
				  command_process(this, command, (Logical)pass);
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
			    UByte response_length = _put_buffer._count;
			    UByte response_header = (response_length << 4) |
			      _put_buffer.checksum(response_length);
			    frame_put(response_header);
			    //trace_char('y');

			    UByte response_index;
			    for (response_index = 0;
			      response_index < response_length;
			      response_index++) {
				data = _put_buffer.ubyte_get();
				frame_put((UShort)data);
			    }
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

		    request_length = 0;
		    trace_char(']');
		    trace_char('\n');
		    trace_char('[');
		}
	    }
	}
    }
}

//Logical Bus::slave_begin(UByte address) {
//    // ...
//
//    trace_char('{');
//
//    // Verify that _get_count = 0 && _put_count = 0:
//    Logical return_result = 0;
//    _get_count = 0;
//    _put_count = 0;
//    _get_total = 0;
//
//    // Wait until we get an address byte:
//    UShort match_address = ((UShort)address) | 0x100;
//    while (frame_get() != match_address) {
//        // No yet, keep waiting:
//    }
//
//    // Give an immediate acknowledge if {address} is an "immediate acknowledge"
//    // address:
//    if ((address & 0x80) == 0) {
//        // This address requires an immediate acknowledge:
//	frame_put(0xa5);
//    }
//
//    // Read the header byte:
//    UShort header_frame = frame_get();
//    _get_total = ((UByte)header_frame) >> 4;
//    //trace_char('!');
//    //trace_hex(_get_total);
//
//    // Read in {length} bytes and compute {checksum} at the same time:
//    UByte index;
//    UByte checksum = 0;
//    for (index = 0; index < _get_total; index++) {
//	UShort frame = frame_get();
//	// Check for address bit:
//	UByte ubyte = (UByte)frame;
//	_get_buffer[index] = ubyte;
//	checksum += ubyte;
//    }
//
//    // Compute the final checksum which is only 4 bits wide:
//    checksum = (checksum + (checksum >> 4)) & 0xf;
//
//    // Verify that check sum matches:
//    if (checksum == (header_frame & 0xf)) {
//	// {checksum} does match:
//        //trace_char('@');
//	return_result = 1;
//	_get_count = 0;
//    } else {
//	// {checksum} does not match; give a negative response:
//        //trace_char('#');
//	frame_put(0xf0);
//    }
//    //trace_char('r');
//    //trace_hex(return_result);
//    //trace_char('!');
//
//    return return_result;
//}
//
//Logical Bus::slave_command_is_pending()
//{
//    //Logical result = (Logical)(_get_count < _get_total);
//    //trace_char('n');
//    //trace_hex(result);
//    //return result;
//    return (Logical)(_get_count < _get_total);
//}
//
//void Bus::slave_end()
//{
//    // Compute checksum:
//    trace_char(':');
//    UByte index;
//    UByte checksum = 0;
//    for (index = 0; index < _put_count; index++) {
//	checksum += _put_buffer[index];
//    }
//    checksum = (checksum + (checksum >> 4)) & 0xf;
//
//    frame_put((_put_count<< 4) | checksum);
//
//    for (index = 0; index < _put_count; index++) {
//	frame_put(_put_buffer[index]);
//    }
//
//    trace_char('}');
//    trace_char('\n');
//}

// {Bus_Module} routines:

//void Bus_Module::master_begin()
//{
//    _bus->begin(_address);
//}

void Bus_Module::bind(Bus *bus, UByte address)
{
    _bus = bus;
    _address = address;
}

//void Bus_Module::master_end()
//{
//    _bus->end();
//}

//UByte *Bus_Module::ubyte_register_get(UByte reg) {
//    _bus->ubyte_put((reg << 1) | 0);
//    return _bus->ubyte_get();
//}

//void Bus_Module::ubyte_register_put(UByte reg, UByte ubyte) {
//    _bus->ubyte_put((reg << 1) | 1);
//    _bus->ubyte_put(ubyte);
//}

//Logical Bus_Module::slave_begin()
//{
//    return _bus->slave_begin(_address);
//}

//Logical Bus_Module::slave_command_is_pending()
//{
//    return _bus->slave_command_is_pending();
//}

//void Bus_Module::slave_end()
//{
//    _bus->slave_end();
//}

// {Foo_Module} routines:

//void Foo_Module::commands_process()
//{
//    while (slave_command_is_pending()) {
//        UByte command = *ubyte_get();
//	//trace_char('c');
//	//trace_hex(command);
//	switch (command) {
//	  case 0:
//	    ubyte_put(_x);
//	    break;
//	  case 1:
//	    _x = *ubyte_get();
//	    break;
//	  case 2:
//	    ubyte_put(_y);
//	    break;
//	  case 3:
//	    _y = *ubyte_get();
//	    break;
//	  default:
//	    break;
//	}
//    }
//}

// {LCD32_Module} stuff
//
//LCD32_Module::LCD32_Module()
//{
//}
//
//Integer LCD32_Module::button() {
//    // This routine will the current button status for {this}.
//
//    command_begin(LCD32__button);
//    flush();
//    UByte byte_result = ubyte_get();
//    Integer result = (Integer)byte_result;
//    if (byte_result == 0xff) {
//	result = KEYPAD_NONE;
//    }
//    command_end();
//    return result;
//}
//
//void LCD32_Module::setCursor(unsigned char column, unsigned char row) {
//    command_begin(LCD32__setCursor);
//    ubyte_put((UByte)column);
//    ubyte_put((UByte)row);
//    command_end();
//}
//
//size_t LCD32_Module::write(UByte ch) {
//    command_begin((UByte)LCD32__writeChar);
//    ubyte_put((UByte)ch);
//    command_end();
//    return 1;
//}
