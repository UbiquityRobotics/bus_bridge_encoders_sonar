//! @file Bus.h
//! @brief Header file for Bus classes
//!
//! This file defines the classes used for Bus communication

#ifndef BUS_H
#define BUS_H


// All typedef's go up here before the #includes':

// To enable bus logging code, get *BUS_DEBUG* define to 1; otherewise 
// set it to 0 to disable debugging:
#define BUS_LOG 0

// Set *BUS_TRACE* to 1 to enable tracing:
#define BUS_TRACE 0

// Signed types:
typedef signed char Byte;		// 8-bit signed byte (-128 ... 128):
typedef char Character;			// 8-bit character (sign whatever)
typedef double Double;			// 64-bit double precision float number
typedef float Float;			// 32-bit single precision float number
typedef signed int Integer;		// 32-bit signed integer
typedef signed long int Long;		// 64-bit signed integer
typedef signed short Short;		// 16-bit signed integer

// Unsigned types start with 'U' character:
typedef unsigned char UByte;		// 8-bits unsigned
typedef unsigned long UInteger;		// 32-bits unsigned
typedef unsigned char Logical;		// 1-bit logical value
typedef unsigned long long int ULong;	// 64-bit unsigned
typedef unsigned short UShort;		// 16-bit unsigned
typedef unsigned short Unicode;		// 16-bit Unicode character

#include "Arduino.h"
#include "Print.h"

class UART {
  public:
    virtual Logical can_transmit() = 0;
    virtual Logical can_receive() = 0;
    virtual UShort frame_get() = 0;
    virtual void frame_put(UShort frame) = 0;
    virtual void interrupt_set(Logical interrupt) = 0;
    void print(UByte ubyte) { uinteger_print((UInteger)ubyte); }
    void print(Character character) { frame_put(character); }
    void print(UShort ushort) { uinteger_print((UInteger)ushort); }
    void print(Character *string) { string_print(string); }
    void string_print(Character *string);
    void uinteger_print(UInteger uinteger);
};

class AVR_UART : public UART {
  public:
    AVR_UART(volatile UByte *ubrrh, volatile UByte *ubrrl,
     volatile UByte *ucsra, volatile UByte *ucsrb, volatile UByte *ucsrc,
     volatile UByte *udr);
    void begin(UInteger frequency,
     UInteger baud_rate, Character *configuration);
    virtual Logical can_receive();
    virtual Logical can_transmit();
    void end();
    virtual UShort frame_get();
    virtual void frame_put(UShort frame);
    virtual void interrupt_set(Logical interrupt);
    void receive_interrupt();
    void reset();
    void transmit_interrupt();
  private:
    UByte static const _ring_power = 4;
    UByte static const _ring_size = 1 << _ring_power;
    UByte static const _ring_mask = _ring_size - 1;
    volatile UByte _get_head;
    UByte _get_ring[_ring_size];
    volatile UByte _get_tail;
    Logical _interrupt;
    volatile UByte _put_head;
    UByte _put_ring[_ring_size];
    volatile UByte _put_tail;
    Character *_configuration;
    UByte volatile *_ubrrh;
    UByte volatile *_ubrrl;
    UByte volatile *_ucsra;
    UByte volatile *_ucsrb;
    UByte volatile *_ucsrc;
    UByte volatile *_udr;
};

class NULL_UART : public UART {
  public:
    Logical can_receive() { return (Logical)1; };
    Logical can_transmit() { return (Logical)1; };
    UShort frame_get() { return 0xffff; };
    void frame_put(UShort frame) { };
    void interrupt_set(Logical interrupt) { };
};

#if defined(UDR0)
  class AVR_UART0 : public AVR_UART {
    public:
      AVR_UART0() :
       AVR_UART(&UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0) { } ;
  };

  extern AVR_UART0 avr_uart0;
#endif // defined(UDR0)

#if defined(UDR1)
  class AVR_UART1 : public AVR_UART {
    public:
      AVR_UART1() :
       AVR_UART(&UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UCSR1C, &UDR1) { } ;
  };

  extern AVR_UART1 avr_uart1;
#endif // defined(UDR1)

// These two defines are only used when *BUS_DEBUG* is set to 1:
#if BUS_DEBUG
    #define BUS_LOG_SIZE 128
    #define BUS_LOG_MASK (BUS_LOG_SIZE - 1)
#endif // BUS_DEBUG

// These defines are empty when *BUS_TRACE* is 0:
#if BUS_TRACE
  #define trace_char(ch) //Serial.write(ch)
  #define trace_hex(hex) //Serial.print(hex, HEX);
#else // BUS_TRACE
  #define trace_char(ch)
  #define trace_hex(hex)
#endif // BUS_TRACE

//! @class Bus_Buffer
//! @brief Send/Receive packet buffer.
//!
//! This is class to hold bus packets that are sent and received.

class Bus_Buffer
{
  public:				// In alphabetical order:
    Bus_Buffer();			// Constructor
    UByte check_sum();			// Compute 4-bit check sum
    void reset();			// Reset/clear buffer
    void show(UByte Tag);		// Show buffer (for debgging)
    UByte ubyte_get();			// Get next *UByte* from buffer
    void ubyte_put(UByte ubyte);	// Put a *UByte* into buffer
    UShort ushort_get();		// Get next *UShort* from buffer
    void ushort_put(UShort ushort);	// Put a *UShort* into buffer

    // Any fields with a preceeding underscore, should only be
    // set byt *Bus_Buffer* methods.  Reading the values is OK:
    UByte _error_flags;			// Error flags go here
    UByte _get_index;			// Next byte to get index
    UByte _put_index;			// Next byte to put index
    static const UByte _ubytes_power = 4; // Bufere size must be power of 2
    static const UByte _ubytes_size = 1 << _ubytes_power; // Actual buffer size
    static const UByte _ubytes_mask = _ubytes_size - 1; // Index mask
    UByte _ubytes[_ubytes_size];	// The actual buffer bytes
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
    Bus(UART *bus_uart, UART *debug_uart);

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
    void command_begin(UByte address, UByte command, UByte put_bytes);

    //! @brief Mark current command as completed.
    //!
    //! This method marks that the current command has ended.  The command
    //! is started by a call to *command_begin*().  In automatic flush mode,
    //! this command will cause the command to be immediately sent
    //! to the desired module.  In non automatic flush mode, the
    //! the command bytes will continue to collect until an explicit
    //! call to *flush*() occurs.
    void command_end();

    //! @brief Send *byte* to currently selected module.
    //!   @param byte byte value to send to command.
    //!
    //! This member function will queue *logical* to be sent to the
    //! currently selected module as part of a multi-byte command.
    void logical_put(Logical logical) {
      // Queue {byte} to be sent off to bus:
      ubyte_put((UByte)logical);
    }

    void auto_flush_set(Logical auto_flush)
     { _auto_flush = auto_flush; };
    Logical can_receive()
     { return _bus_uart->can_receive(); };
    Logical can_transmit()
     { return _bus_uart->can_transmit(); };
    UByte command_ubyte_get(UByte address, UByte command);
    void command_ubyte_put(UByte address, UByte command, UByte ubyte);
    UShort frame_get()
     { return _bus_uart->frame_get(); };
    void frame_put(UShort frame)
     { _bus_uart->frame_put(frame); };
    Logical flush();
    void interrupt_set(Logical interrupt)
     { _bus_uart->interrupt_set(interrupt); };

    Logical logical_get();

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

    static const UByte _get_ring_power = 4;
    static const UByte _get_ring_size = 1 << _get_ring_power;
    static const UByte _get_ring_mask = _get_ring_size - 1;
    UShort _get_ring[_get_ring_size];	// Ring buffer for received frames
    volatile UByte _get_head;
    volatile UByte _get_tail;

    static const UByte _put_ring_power = 4;
    static const UByte _put_ring_size = 1 << _put_ring_power;
    static const UByte _put_ring_mask = _put_ring_size - 1;
    UShort _put_ring[_put_ring_size];	// Ring buffer for received frames
    volatile UByte _put_head;
    volatile UByte _put_tail;

    UShort _echo_suppress;	// Frame to suppress; (OR in 0x8000 to suppress)

    Logical put_buffer_send();

  private:
    static const UByte _maximum_request_size = 15;

    UART *_bus_uart;		// UART connected to bus
    UART *_debug_uart;		// UART used for debugging messages
    Bus_Buffer _get_buffer;	// FIFO for received bytes
    Bus_Buffer _put_buffer;	// FIFO queue for bytes to send

    Logical _auto_flush;	// 1=>Auto flush every cmd; 0=>queue up cmds
    //Logical _master_mode;	// 1=>master mode; 0=>slave mode 
    UShort _desired_address;	// Desired address
    UShort _current_address;	// Current address

    // The frame log is only enabled when *BUS_DEBUG* is set to 1:
    #if BUS_DEBUG
      UShort _log_buffer[BUS_LOG_SIZE];	// Buffer of read/written frames
      UByte _log_total;			// Total number read/written
      UByte _log_dumped;		// Total number dumped out
    #endif // BUS_DEBUG
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

    void command_begin(UByte command, UByte put_bytes) {
      // This routine will start a new bus command that starts
      // with the byte {command}.  This command is sent to the
      // current module selected by {this}.

      _bus->command_begin(_address, command, put_bytes);
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

    void auto_flush_set(Logical auto_flush) {
      // This routine will Set the automatic flush mode for {this}
      // to {auto_flush}.  If {auto_flush} is 1, each command as
      // flushed as soon as possible.  If {auto_flush} is 0,
      // commands are queued until an explicit flush occurs.
      // The previous value of flush mode is returned.

      _bus->auto_flush_set(auto_flush);
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
