#include "battery.h"

battery::battery (uint8_t adc_pin)
{
    m_adc_pin = adc_pin;
    adcAttachPin(m_adc_pin);
}

uint8_t battery::get_battery_level ()
{
    m_adc_val = analogRead(m_adc_pin);
    if (m_adc_val < 2050)
        return LEVEL_1;
    else if ((m_adc_val < 2150 ) && (m_adc_val >= 2050))
        return LEVEL_2;
    else if ((m_adc_val < 2250) && (m_adc_val >= 2150))
        return LEVEl_3;
    else if (m_adc_val >= 2250)
        return LEVEL_4;
}