#include "oled.h"



OLED::OLED(uint8_t w, uint8_t h):
    Adafruit_SSD1306(w, h)
{}


void OLED::print_battery_level(uint16_t PinV)
{
    drawRect(117, 0, 11, 6, WHITE);
    fillRect(115, 2, 2, 2, WHITE);
    if((PinV >= 1) && (PinV <= 3))
    {
        fillRect(124, 1, 3, 4, WHITE);
    }
    if((PinV >= 2) && (PinV <= 3))
    {
        fillRect(121, 1, 3, 4, WHITE);
    }
    if(PinV == 3)
    {
        fillRect(118, 1, 3, 4, WHITE);
    }
}


void OLED::print_bluetooth_paired_status (bool status)
{
    if(status)
    {
        print("BT Connected");
    }
    else
    {
        print("BT Disconnected");
    }
}


void OLED::print_GNSS_status(uint8_t FixType, uint8_t RTKType)
{
    switch (FixType)
    {
        case(0):
            print("No Fix");
            break;
        case(1):
            print("DR");
            break;
        case(2):
            print("2D");
            break;
        case(3):
            print("3D");
            break;
        case(4):
            print("GNSS DR");
            break;
        case(5):
            print("Time");
            break;
        default:
            print("No Fix");
            break;
    }
    switch (RTKType)
    {
        case('F'):
            print(" RTK Float");
            break;
        case('R'):
            print(" RTK Fixed");
            break;
    }
}
