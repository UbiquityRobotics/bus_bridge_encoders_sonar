#include "Arduino.h"
#include "Bus.h"

#define TRACE

#ifdef TRACE
#define trace_char(ch) Serial.write(ch)
#define trace_hex(hex) Serial.print(hex, HEX);
#else
#define trace_char(ch)
#define trace_hex(hex)
#endif

#define ZAP
#ifdef ZAP

void serialEvent1() __attribute__((weak));
void serialEvent1() {}

//void serialEventRun(void)
//{
//    if (Serial1.available()) {
//	serialEvent1();
//    }
//}

// Interrupt handler for UART1 Receive:
SIGNAL(USART1_RX_vect)
{
    //trace_char('[');
    // Read the 9th bit first, because reading UDR1 clears the 9th bit flag:
    UShort frame = 0;
    if (bit_is_set(UCSR1B, RXB81)) {
	frame = 0x100;
    }

    // Read the remaining 8-bits out:
    frame |= (UShort)UDR1;

    // On an address select, we always reset the bus input buffer:
    if ((frame & 0x100) != 0) {
	bus._get_tail = 0;
	bus._get_head = 0;
	bus._echo_suppress = 0;
    }

    // When *echo_suppress* is enabled, we wait until we get a match:
    UShort echo_suppress = bus._echo_suppress;
    if ((echo_suppress & 0x8000) != 0) {
	// Ignore frames until it matches {echo_suppress};
	if ((echo_suppress & 0x1ff) == frame) {
	    // We have an echo match:
	    bus._echo_suppress = 0;
	} // else keep on suppressing...
    } else {
	// We have to deal with this frame, since it is not an echo:
	UByte get_head = (bus._get_head + 1) & GET_RING_MASK;
	if (get_head != bus._get_tail) {
	    // Stuff frame into buffer and update buffer total:
	    bus._get_ring[get_head] = frame;
	    bus._get_head = get_head;
	}

	// For now, leave Multi-Processor Communication Mode disabled:
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
    //trace_char(']');
}

// Interrupt handler for UART1 Data Register Empty flag:
ISR(USART1_UDRE_vect)
{
    //trace_char('[');

    // The transmit buffer is ready for a new frame.  Now check to
    // see if we have a frame to feed it:
    UByte put_tail = bus._put_tail;
    if (bus._put_head == put_tail) {
	// Put ring buffer is empty, so disable interrupts:
	UCSR1B &= ~_BV(UDRIE1);
    } else {
	// Buffer is not empty so send the next byte on its way:
	UShort frame = bus._put_ring[put_tail];
	bus._put_tail = (put_tail + 1) % PUT_RING_MASK;

	// Output the frame.  Set 9th bit first, followed by remaining 8 bits:
	UCSR1B &= ~_BV(TXB81);
	if ((frame & 0x100) != 0) {
	    UCSR1B |= _BV(TXB81);
	}

	// Send next byte:
	UDR1 = (UByte)frame;

	// Supress echo:
	bus._echo_suppress = frame | 0x8000;
    }
    //trace_char(']');
}

#endif //ZAP


/*
 */

Bus::Bus() {
    // We want to do the following to the USART:
    //  - Set transmit/receive rate to 1Mbps
    //  - Single rate transmission (Register A)
    //  - All interrupts disabled for now (Register B)
    //  - Transmit enable (Register B)
    //  - Receive enable (Register B)
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

    // Initialize USART1 baud rate to 1Mbps:
    //     f_cpu = 16000000L
    //     baud_rate = 1000000L
    //     uubrr1 = f_cpu / (baud_rate * 16L) - 1
    //            = 16000000 / (1000000 * 16) - 1
    //            = 16000000 / 16000000 - 1
    //            = 1 - 1
    //            = 0
    UBRR1L = 0;
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
    //	    m: Multi-processor communication mode
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
    _slave_address = 0xffff;
    _address = 0;
    _put_count = 0;
    _get_count = 0;
    _last_address = 0xff;

    _get_head = 0;
    _get_tail = 0;
    _put_head = 0;
    _put_tail = 0;

    UCSR1A &= ~_BV(MPCM1);
    UCSR1B |= _BV(RXEN1);

    // UByte zilch = UDR1;
    // zilch = UDR1;
}

void Bus::begin(UByte address) {
    // This routine will start to buffer up commands to be sent to
    // the module on the bus that matches {address}.

    trace_char('<');

    _address = address;
    // Check that _put_count and _get_count are zero:
    _put_count = 0;
    _get_count = 0;
}

UInteger Bus::uinteger_get() {
    UInteger uinteger = 
      (((UInteger)_get_buffer[_get_count]) << 24) |
      (((UInteger)_get_buffer[_get_count + 1]) << 16) |
      (((UInteger)_get_buffer[_get_count + 2]) << 8) |
      (UInteger)_get_buffer[_get_count + 3];
    _get_count += sizeof(UInteger);
    return uinteger;
}

void Bus::uinteger_put(UInteger uinteger) {
    // ...

    _put_buffer[_put_count] = (UByte)(uinteger >> 24);
    _put_buffer[_put_count + 1] = (UByte)(uinteger >> 16);
    _put_buffer[_put_count + 2] = (UByte)(uinteger >> 8);
    _put_buffer[_put_count + 3] = (UByte)uinteger;
    _put_count += sizeof(UInteger);
}

UShort Bus::ushort_get() {
    //...

    UShort ushort =
      (((UInteger)_get_buffer[_get_count]) << 8) |
      (UInteger)_get_buffer[_get_count + 1];
    _get_count += sizeof(UShort);
    return ushort;
}

void Bus::ushort_put(UShort ushort) {
    // ...

    _put_buffer[_put_count] = (UByte)(ushort >> 8);
    _put_buffer[_put_count + 1] = (UByte)ushort;
    _put_count += sizeof(UShort);
}

UByte Bus::ubyte_get() {
    // ...

    return (UByte)_get_buffer[_get_count++];
}

void Bus::ubyte_put(UByte ubyte) {
    // ...

    _put_buffer[_put_count++] = ubyte;
}

void Bus::end() {
    // Flush current buffer:

    // Try 3 times:
    UByte flush_count;
    for (flush_count = 0; flush_count < 3; flush_count++) {
	trace_char('|');

	// Determine if a new module address needs to be selected:
	//if (_last_address != _address) {
	if (1) {
	    frame_put(((UShort)_address) | 0x100);
	    if ((_address & 0x100) != 0) {
		// Verify that we get the right acknowledge back:
	        (void)frame_get();
	    }
	    _last_address = _address;
	}

	// Compute the checksum:
	UByte checksum = 0;
	UByte index = 0;
	for (index = 0; index < _put_count; index++) {
	    checksum += _put_buffer[index];
	}

	// Output the length/checksum byte:
	checksum = (checksum + (checksum >> 4)) & 0xf;
	frame_put((_put_count << 4) | checksum);

	// Output the {_put_count} bytes from {_put_buffer}:
	for (index = 0; index < _put_count; index++) {
	    frame_put(_put_buffer[index]);
	}

	// Wait for the ack:
	UShort acknowledge = frame_get();
	UByte result_checksum = acknowledge & 0xf;
	UByte result_length = acknowledge >> 4;
	if (result_length == _get_count) {
	    checksum = 0;
	    for (index = 0; index < _get_count; index++) {
		UByte byte = (UByte)frame_get();
		_get_buffer[index++] = byte;
		checksum += byte;
	    }
	    checksum = (checksum + (checksum >> 4)) & 0xf;
	    if (checksum == result_checksum) {
		break;
	    }
	} else {
	    // Expectation mismatch:
	    frame_put(0x1ff);
	}
    }

    // Mark that the buffers are "empty":
    _put_count = 0;
    _get_count = 0;

    trace_char('>');
    trace_char('\n');
}

UShort Bus::frame_get() {
    // Wait for a 9-bit frame to arrive and return it:

    trace_char('g');
    UCSR1B |= _BV(RXEN1);
    UShort frame = 0;
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

Logical Bus::slave_begin(UByte address) {
    // ...

    trace_char('{');

    // Verify that _get_count = 0 && _put_count = 0:
    Logical return_result = 0;
    _get_count = 0;
    _put_count = 0;
    _get_total = 0;

    // Wait until we get an address byte:
    UShort match_address = ((UShort)address) | 0x100;
    while (frame_get() != match_address) {
        // No yet, keep waiting:
    }

    // Give an immediate acknowledge if {address} is an "immediate acknowledge"
    // address:
    if ((address & 0x80) == 1) {
        // This address requires an immediate acknowledge:
	frame_put(0xa5);
    }

    // Read the header byte:
    UShort header_frame = frame_get();
    _get_total = ((UByte)header_frame) >> 4;
    trace_char('!');
    trace_hex(_get_total);

    // Read in {length} bytes and compute {checksum} at the same time:
    UByte index;
    UByte checksum = 0;
    for (index = 0; index < _get_total; index++) {
	UShort frame = frame_get();
	// Check for address bit:
	UByte ubyte = (UByte)frame;
	_get_buffer[index] = ubyte;
	checksum += ubyte;
    }

    // Compute the final checksum which is only 4 bits wide:
    checksum = (checksum + (checksum >> 4)) & 0xf;

    // Verify that check sum matches:
    if (checksum == (header_frame & 0xf)) {
	// {checksum} does match:
        trace_char('@');
	return_result = 1;
	_get_count = 0;
    } else {
	// {checksum} does not match; give a negative response:
        trace_char('#');
	frame_put(0xf0);
    }
    trace_char('r');
    trace_hex(return_result);
    trace_char('!');

    return return_result;
}

Logical Bus::slave_command_is_pending()
{
    Logical result = (Logical)(_get_count < _get_total);
    trace_char('n');
    trace_hex(result);
    return result;
}

void Bus::slave_end()
{
    // Compute checksum:
    UByte index;
    UByte checksum = 0;
    for (index = 0; index < _put_count; index++) {
	checksum += _put_buffer[index];
    }
    checksum = (checksum + (checksum >> 4)) & 0xf;

    frame_put((_put_count<< 4) | checksum);

    for (index = 0; index < _put_count; index++) {
	frame_put(_put_buffer[index]);
    }

    trace_char('}');
    trace_char('\n');
}

void Bus::slave_mode(UByte address,
  UByte (*command_process)(Bus *, UByte, Logical)) {
}

// {Bus_Module} routines:

void Bus_Module::begin()
{
    _bus->begin(_address);
}

void Bus_Module::bind(Bus *bus, UByte address)
{
    _bus = bus;
    _address = address;
}

void Bus_Module::end()
{
    _bus->end();
}

UByte Bus_Module::ubyte_register_get(UByte reg) {
    _bus->ubyte_put((reg << 1) | 0);
    return _bus->ubyte_get();
}

void Bus_Module::ubyte_register_put(UByte reg, UByte ubyte) {
    _bus->byte_put((reg << 1) | 1);
    _bus->byte_put(ubyte);
}

Logical Bus_Module::slave_begin()
{
    return _bus->slave_begin(_address);
}

Logical Bus_Module::slave_command_is_pending()
{
    return _bus->slave_command_is_pending();
}

void Bus_Module::slave_end()
{
    _bus->slave_end();
}

