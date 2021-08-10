#ifndef _BATTERRY_H_
#define _BATTERRY_H_

#include <Arduino.h>
//Define battery level
#define LEVEL_1 0
#define LEVEL_2 1
#define LEVEl_3 2
#define LEVEL_4 3

class battery {
public: 
    battery(uint8_t adc_pin);
    uint8_t get_battery_level ();
private:
    uint8_t m_adc_pin; 
    uint16_t m_adc_val;
};

#endif