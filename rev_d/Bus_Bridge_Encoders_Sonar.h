// Generated file: only edit in designated area!

#ifndef MOTOR3_LOCAL_H
#define MOTOR3_LOCAL_H

//#include <MB7.h>
#include <Bus.h>

  //////// Edit begins here: TOP_LEVEL
  //////// Edit ends here: TOP_LEVEL

class Bus_Bridge_Encoders_Sonar : public Bus_Module {
  public:
    // Constructor
    Bus_Bridge_Encoders_Sonar(UByte address);

    Logical led_get();
    void led_set(Logical led);

    // Motor speed
    Byte speed_get();
    void speed_set(Byte speed);

    // Invert motor direction
    Logical direction_invert_get();
    void direction_invert_set(Logical direction_invert);

    // 8-bit Motor encoder
    Byte encoder8_get();
    void encoder8_set(Byte encoder8);

    // Motor encoder
    Integer encoder_get();
    void encoder_set(Integer encoder);

    UByte _address;
    Logical _led;

  //////// Edit begins here: PRIVATE
  //////// Edit ends here: PRIVATE
};

#endif // MOTOR3_LOCAL_H
