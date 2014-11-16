/*
 
 */

#ifndef Bus_h
#define Bus_h

#include "Arduino.h"

// Signed types:
typedef signed char Byte;		// 8-bit signed byte (-128 ... 128):
typedef float Float;			// 32-bit single precision float number
typedef signed long Integer;		// 32-bit signed integer
typedef unsigned char Logical;		// 8-bit logical value
typedef signed short Short;		// 16-bit signed integer

// Unsigned types start with 'U' character:
typedef unsigned char UByte;		// 8-bits unsigned
typedef unsigned long UInteger;		// 32-bits unsigned
typedef unsigned short UShort;		// 16-bit unsigned

// Size of ring buffer must be power of 2
#define GET_RING_SIZE 16
#define GET_RING_MASK (GET_RING_SIZE - 1)
#define PUT_RING_SIZE 16
#define PUT_RING_MASK (PUT_RING_SIZE - 1)

class Bus
{
  public:
    Bus();
    void begin(UByte address);
    void end();
    UShort frame_get();
    void frame_put(UShort);

    // *Byte*/*UByte* routines:
    Byte byte_get() {
	return (Byte)ubyte_get();
    }
    void byte_put(Byte byte) {
	ubyte_put((UByte)byte);
    }
    UByte ubyte_get();
    void ubyte_put(UByte ubyte);

    // *Integer*/*UInteger* routines:
    Integer integer_get() {
	return (Integer)uinteger_get();
    }
    void integer_put(Integer integer) {
	uinteger_put((UInteger)integer);
    }
    UInteger uinteger_get();
    void uinteger_put(UInteger uinteger);

    // *Logical* routines:
    Logical logical_get() {
	return (Logical)ubyte_get();
    }
    void logical_put(Logical logical) {
	ubyte_put((UByte)logical);
    }

    // *Short*/*UShort* routines:
    Short short_get() {
	return (Short)ushort_get();
    }
    void short_put(Short sshort) {
	ushort_put((UShort)sshort);
    }
    UShort ushort_get();
    void ushort_put(UShort ushort);

    // Slave member functions:
    void slave_mode(UByte address,
      UByte (*command_process)(Bus *, UByte, Logical));
    Logical slave_begin(UByte address);
    Logical slave_command_is_pending();
    void slave_end();

    //  private:
    UShort _echo_suppress;	// Frame to suppress; (OR in 0x8000 to suppress)
    Logical _master_mode;	// 1=>master mode; 0=>slave mode
    UByte _address;		// Currently selected address;
    UByte _last_address;	// Last selected address
    UShort _slave_address;	// Slave address to match as a slave

    UShort _get_ring[GET_RING_SIZE];	// Ring buffer for received frames
    volatile UByte _get_head;
    volatile UByte _get_tail;

    UShort _put_ring[PUT_RING_SIZE];	// Ring buffer for received frames
    volatile UByte _put_head;
    volatile UByte _put_tail;

    UByte _get_buffer[16];	// Get buffer
    UByte _get_count;		// Number of bytes in buffer
    UByte _get_total;

    UByte _put_buffer[16];	// Put buffer
    UByte _put_count;		// Current count of bytes in buffer
    UByte _put_index;
    UByte _put_total;
};

class Bus_Module
{
  public:
    Bus_Module()
	{ _bus = (Bus *)0; _address = 0; }
    void begin();
    void bind(Bus *maker_bus, UByte address);
    void end();

    UByte ubyte_register_get(UByte reg);
    void ubyte_register_put(UByte reg, UByte ubyte);

    UByte ubyte_get()
        { return _bus->ubyte_get(); }
    void ubyte_put(UByte ubyte)
	{ _bus->ubyte_put(ubyte); }

    Logical slave_begin();
    Logical slave_command_is_pending();
    void slave_end();

  private:
    Bus *_bus;
    UByte _address;
};

extern Bus bus;

#endif // Bus_h
