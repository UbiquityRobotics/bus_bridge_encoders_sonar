//! @file MB7.h
//! @brief Header file for MB6 classes
//!
//! This file defines the classes used by MB6

#ifndef BUS_H
#define BUS_H

#include "Arduino.h"
#include "Print.h"

// To enable debugging code, get {BUS_DEBUG} define to 1; otherewise 
// set it to 0 to disable debugging:
#define BUS_DEBUG 0

#define TRACE

#ifdef TRACE
#define trace_char(ch) Serial.write(ch)
#define trace_hex(hex) Serial.print(hex, HEX);
#else
#define trace_char(ch)
#define trace_hex(hex)
#endif

typedef char Character;			// 8-bit character (sign whatever)

// Signed types:
typedef signed char Byte;		// 8-bit signed byte (-128 ... 128):
typedef double Double;			// 64-bit double precision float number
typedef float Float;			// 32-bit single precision float number
typedef signed int Integer;		// 32-bit signed integer
typedef signed long int Long;		// 64-bit signed integer
typedef signed short Short;		// 16-bit signed integer

// Unsigned types start with 'U' character:
typedef unsigned char UByte;		// 8-bits unsigned
typedef unsigned int UInteger;		// 32-bits unsigned
typedef unsigned char Logical;		// 1-bit logical value
typedef unsigned long int ULong;	// 64-bit unsigned
typedef unsigned short UShort;		// 16-bit unsigned
typedef unsigned short Unicode;		// 16-bit Unicode character

// Size of ring buffer must be power of 2:
#define GET_RING_SIZE 16
#define GET_RING_MASK (GET_RING_SIZE - 1)
#define PUT_RING_SIZE 16
#define PUT_RING_MASK (PUT_RING_SIZE - 1)

// These two defines are only used when {BUS_DEBUG} is set to 1:
#define BUS_LOG_SIZE 128
#define BUS_LOG_MASK (BUS_LOG_SIZE - 1)

static const UByte Bus_Buffer__size = 32;
static const UByte Bus_Buffer__mask =  Bus_Buffer__size - 1;
class Bus_Buffer
{
  public:
    Bus_Buffer();
    UByte checksum(UByte count);
    void reset();
    void show(UByte Tag);
    UByte ubyte_get();
    void ubyte_put(UByte ubyte);
    UShort ushort_get();
    void ushort_put(UShort ushort);

    UByte _ubytes[Bus_Buffer__size];
    UByte _get_index;
    UByte _put_index;
    UByte _count;
    UByte _error_flags;
};

//! \class Bus
//! \brief managing the Bus UART
//!
//! This a helper class that takes care of the UART that talks to the
//! Bus.

class Bus
{
  public:
    //! @brief Constructor for Bus object.
    Bus();

    //! @brief Return the a signed byte from currently selected module.
    //!   @return the next signed byte from the command.
    //!
    //! ...
    Byte byte_get() {
	// Return the next {Byte} from recieve buffer:
	return (Byte)ubyte_get();
    }

    //! @brief Send *byte* to currently selected module.
    //!   @param byte byte value to send to command.
    //!
    //! This member function will queue *byte* to be sent to the
    //! currently selected module as part of a multi-byte command.
    //! *byte* is signed.
    void byte_put(Byte byte) {
	// Queue {byte} to be sent off to bus:
	ubyte_put((UByte)byte);
    }

    Logical can_receive();
    Logical can_transmit();

    Logical character_get() {
	// Return the next {character} from recieve buffer:
	return (Character)ubyte_get();
    }

    //! @brief Send *byte* to currently selected module.
    //!   @param character Character value to send to command.
    //!
    //! This member function will queue *character* to be sent to the
    //! currently selected module as part of a multi-byte command.
    void character_put(Character character) {
	// Queue {character} to be sent off to bus:
	ubyte_put((UByte)character);
    }

    //! @brief Send *command* to the module at *address*.
    //!
    //!   @param address address (0 - 254) of module to connect to
    //!   @param command first byte of command
    //!
    //! This mebmer function will cause the module at *address* to be
    //! selected if it has not already been selected.   A sequence of
    //! 1 or more bytes is sent to the selected module followed by 0,
    //! 1 or more returned bytes.  The final call is *command_end*().
    //! The first byte sent to the selected module is *command*.
    void command_begin(UByte address, UByte command);

    //! @brief Mark current command as completed.
    //!
    //! This method marks that the current command has ended.  The command
    //! is started by a call to *command_begin*().  In automatic flush mode,
    //! this command will cause the command to be immediately sent
    //! to the desired module.  In non automatic flush mode, the
    //! the command bytes will continue to collect until an explicit
    //! call to *flush*() occurs.
    void command_end();

    Logical logical_get() {
	// Return the next {Byte} from recieve buffer:
	return (Logical)ubyte_get();
    }

    //! @brief Send *byte* to currently selected module.
    //!   @param byte byte value to send to command.
    //!
    //! This member function will queue *logical* to be sent to the
    //! currently selected module as part of a multi-byte command.
    void logical_put(Logical logical) {
	// Queue {byte} to be sent off to bus:
	ubyte_put((UByte)logical);
    }

    void mode_set(Logical poll_mode);
    UShort frame_get();
    void frame_put(UShort);
    void flush();
    Logical flush_mode(Logical auto_flush);

    //! @brief Return next byte from currently selected module.
    //!   @return the next byte from currently selected module.
    //!
    //! This method will return the next unsigned byte from the currently
    //! selected module.
    UByte ubyte_get();

    //! @brief Send *ubyte* to currently selected module.
    //!    @param ubyte	unsigned byte to send
    //!
    //! This method will send *ubyte* to the currently selected module.
    void ubyte_put(UByte ubyte);
	// Queue {ubyte} to be sent off to bus:

    //! @brief Return next short from currently selected module.
    //!   @return the next short from currently selected module.
    //!
    //! This method will return the next unsigned short from the currently
    //! selected module.
    UShort ushort_get();

    //! @brief Send *ushort* to currently selected module.
    //!    @param ushort	unsigned short to send
    //!
    //! This method will send *ushort* to the currently selected module.
    void ushort_put(UShort ushort);
	// Queue {ushort} to be sent off to bus:

    //! @brief Handle bus communication for a module in slave mode.
    //!   @param address Module address to listen to
    //!   @param command_process helper routine to process each
    //!                          byte of received from master
    void slave_mode(UByte address, UByte (*command_process)
     (Bus *bus, UByte command, Logical mode));

    // The {log_dump} method is only enabled when BUS_DEUBG is set to 1:
    #if BUS_DEBUG != 0
	// Dump frame buffer:
	void log_dump();
    #endif // BUS_DEBUG != 0

    // The following fields should be private, but the interrupt
    // service routine needs to be able to access them.  Trying to figure
    // out how to do this with a friend fuction to a function with C linkage
    // was too hard:

    UShort _get_ring[GET_RING_SIZE];	// Ring buffer for received frames
    volatile UByte _get_head;
    volatile UByte _get_tail;

    UShort _put_ring[PUT_RING_SIZE];	// Ring buffer for received frames
    volatile UByte _put_head;
    volatile UByte _put_tail;

    UShort _echo_suppress;	// Frame to suppress; (OR in 0x8000 to suppress)

  private:
    Bus_Buffer _get_buffer;	// FIFO for received bytes
    Bus_Buffer _put_buffer;	// FIFO queue for bytes to send
    Logical _poll_mode;		// 1=>poll; 0=>use interrupts
    Logical _auto_flush;	// 1=>Auto flush every cmd; 0=>queue up cmds
    Logical _master_mode;	// 1=>master mode; 0=>slave mode
    UByte _address;		// Currently selected address;
    UByte _last_address;	// Last selected address
    //UShort _slave_address;	// Slave address to match as a slave

    UByte _commands_length;	// No. of valid command bytes in {_put_buffer}

    // The frame log is only enabled when {BUS_DEBUG} is set to 1:
    #if BUS_DEBUG != 0
	UShort _log_buffer[BUS_LOG_SIZE];	// Buffer of read/written frames
	UByte _log_total;	// Total number read/written
	UByte _log_dumped;	// Total number dumped out
    #endif // BUS_DEBUG != 0
};

class Bus_Module
{
  public:
    Bus_Module() {
	// Construct an empty module:
	_bus = (Bus *)0;
	_address = 0xff;
    }

    void bind(Bus *bus, UByte address);
	// Slave module is located at {address} on {bus}:

    Byte byte_get() {
	// This routine will return next next {Byte} from the current
	// module selected by {this}.

	return (Byte)_bus->ubyte_get();
    }

    void byte_put(Byte byte) {
	// This routine will send {byte} to the current module
	// selected by {this}.

 	_bus->ubyte_put((UByte)byte);
    }

    Character character_get() {
	// This routine will return next {Character} from current
	// module selected by {this}.

	return (Logical)_bus->ubyte_get();
    }

    void character_put(Character character) {
	// This routine will send {character} to the current module
	// selected by {this}.

	_bus->ubyte_put((UByte)character);
    }

    void command_begin(UByte command) {
        // This routine will start a new bus command that starts
	// with the byte {command}.  This command is sent to the
	// current module selected by {this}.

	_bus->command_begin(_address, command);
    }

    void command_end() {
	// This routine will end the current command being sent to
	// the current module selected by {this}.

	_bus->command_end();
    }

    void flush() {
	// This routine will flush all pending commands to the current
	// module selected by {this}.

        _bus->flush();
    }

    Logical flush_mode_set(Logical auto_flush) {
	// This routine will Set the automatic flush mode for {this}
	// to {auto_flush}.  If {auto_flush} is 1, each command as
	// flushed as soon as possible.  If {auto_flush} is 0,
	// commands are queued until an explicit flush occurs.
	// The previous value of flush mode is returned.

	return _bus->flush_mode(auto_flush);
    }

    Logical logical_get() {
	// This routine will return next next byte from current
	// module selected by {this}.

	return (Logical)_bus->ubyte_get();
    }

    void logical_put(Logical logical) {
	// Send {logical} to slave:
	_bus->ubyte_put((UByte)logical);
    }

    UByte ubyte_get() {
	// This routine will return next next byte from current
	// module selected by {this}.

        return _bus->ubyte_get();
    }

    void ubyte_put(UByte ubyte) {
	// Send {ubyte} to slave:
	_bus->ubyte_put(ubyte);
    }

    void slave_mode(UByte address,
      UByte (*command_process)(Bus *, UByte, Logical)) {
        return _bus->slave_mode(address, command_process);
    }

  private:
    Bus *_bus;
    UByte _address;
};

extern Bus bus;

#endif // BUS_H
