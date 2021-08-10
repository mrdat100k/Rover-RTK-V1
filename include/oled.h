#ifndef _OLED_H_
#define _OLED_H_

#include <Arduino.h>
#include "Adafruit_SSD1306.h"
#include "ZED_F9P.h"


class OLED: public Adafruit_SSD1306
{
  public:

    OLED(uint8_t w, uint8_t h);

    void print_battery_level(uint16_t PinV);

    void print_bluetooth_paired_status (bool status);

    void print_GNSS_status(uint8_t FixType, uint8_t RTKType);
    

  private:


};


#endif  /* _OLED_H_ */
