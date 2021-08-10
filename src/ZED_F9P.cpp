#include "Zed_F9P.h"

ZED_F9P::ZED_F9P(int uart_nr, void * buff, uint8_t len):
  PUBX(buff,len),
  HardwareSerial(uart_nr)
{}

void ZED_F9P::begin (unsigned long baud)
{
  HardwareSerial::begin (baud);//begin uart for transmition with zed-f9p
  SFE_UBLOX_GPS::begin ();//start transitting through i2c port
}


bool ZED_F9P::config_GNSS()
{
    bool response = true;
    uint8_t maxWait = 250;

    //ZED_F9P UART1 will receive RTCM message from phone via ESP32 and send NMEA message to phone via ESP32
    getPortSettings(COM_PORT_UART1, maxWait);
    if (payloadCfg[OUTPUT_SETTING] != (COM_TYPE_NMEA) || payloadCfg[INPUT_SETTING] != COM_TYPE_RTCM3 )
    {
        response &= setPortOutput (COM_PORT_UART1, COM_TYPE_NMEA);
        response &= setPortInput (COM_PORT_UART1, COM_TYPE_RTCM3);
    }

    //ZED-F9P will receive and send UBX message
    getPortSettings (COM_PORT_I2C, maxWait);
    if (payloadCfg[OUTPUT_SETTING] != (COM_TYPE_UBX) || payloadCfg[INPUT_SETTING] != (COM_TYPE_UBX))
    {
        response &= setPortOutput (COM_PORT_I2C, COM_TYPE_UBX);
        response &= setPortInput (COM_PORT_I2C, COM_TYPE_UBX);
    }

    getPortSettings(COM_PORT_SPI); //Load the settingPayload with this port's settings
    if (payloadCfg[OUTPUT_SETTING] != 0 || payloadCfg[INPUT_SETTING] != 0)
    {
        response &= setPortOutput(COM_PORT_SPI, 0); //Disable all protocols
        response &= setPortInput(COM_PORT_SPI, 0); //Disable all protocols
    }

    //Load the settingPayload with this port's settings
    getPortSettings(COM_PORT_UART2); 
    if (payloadCfg[OUTPUT_SETTING] != 0 || payloadCfg[INPUT_SETTING] != 0)
    {
        response &= setPortOutput(COM_PORT_UART2, 0); //Disable all protocols
        response &= setPortInput(COM_PORT_UART2, 0); //Disable all protocols
    }
  //The USB port on the ZED may be used for RTCM to/from the computer (as an NTRIP caster or client)
  //So let's be sure all protocols are on for the USB port
    getPortSettings(COM_PORT_USB); //Load the settingPayload with this port's settings
    if (payloadCfg[OUTPUT_SETTING] != (COM_TYPE_UBX | COM_TYPE_NMEA | COM_TYPE_RTCM3) || payloadCfg[INPUT_SETTING] != (COM_TYPE_UBX | COM_TYPE_NMEA | COM_TYPE_RTCM3))
    {
        response &= setPortOutput(COM_PORT_USB, (COM_TYPE_UBX | COM_TYPE_NMEA | COM_TYPE_RTCM3)); //Set the USB port to everything
        response &= setPortInput(COM_PORT_USB, (COM_TYPE_UBX | COM_TYPE_NMEA | COM_TYPE_RTCM3)); //Set the USB port to everything
    }

    response &= enableNMEASentences (COM_PORT_UART1);

    response &= setAutoPVT(true,false);
    //response &= setAutoHPPOSLLH(true, false);

    if (getSerialRate (COM_PORT_UART1) != settings.dataPortBaud)
    {
        setSerialRate (settings.dataPortBaud, COM_PORT_UART1);
    }

    if (response == false)
    {
        Serial.println ("Module failed initial config");
        return (false);
    }
    return response;
} 


// uint8_t ZED_F9P::get_GNSS_status()
// {

// }


int ZED_F9P::get_latitude()
{
  int32_t Lat = getLatitude();
  uint8_t LatHP = getHighResLatitudeHp();

  return (Lat*10 + LatHP);
}


int ZED_F9P::get_longitude()
{
  int32_t Lon = getLongitude();
  uint8_t LonHP = getHighResLongitudeHp();

  return (Lon*10 + LonHP);
}


bool ZED_F9P::enableNMEASentences (uint8_t portType)
{
    bool response = true;
    if (settings.outputSentenceGGA == true)
    {
        if (getNMEASettings(UBX_NMEA_GGA, portType) != 1)
            response &= enableNMEAMessage(UBX_NMEA_GGA, portType);
    }
    else if (settings.outputSentenceGGA == false)
    {
      if (getNMEASettings(UBX_NMEA_GGA, portType) != 0)
        response &= disableNMEAMessage(UBX_NMEA_GGA, portType);
    }

    if (settings.outputSentenceGSA == true)
    {
        if (getNMEASettings(UBX_NMEA_GSA, portType) != 1)
            response &= enableNMEAMessage(UBX_NMEA_GSA, portType);
    }
    else if (settings.outputSentenceGSA == false)
    {
      if (getNMEASettings(UBX_NMEA_GSA, portType) != 0)
        response &= disableNMEAMessage(UBX_NMEA_GSA, portType);
    }
  //When receiving 15+ satellite information, the GxGSV sentences can be a large amount of data
  //If the update rate is >1Hz then this data can overcome the BT capabilities causing timeouts and lag
  //So we set the GSV sentence to 1Hz regardless of update rate
    if (settings.outputSentenceGSV == true)
    {
        if (getNMEASettings(UBX_NMEA_GSV, portType) != settings.gnssMeasurementFrequency)
            response &= enableNMEAMessage(UBX_NMEA_GSV, portType, settings.gnssMeasurementFrequency);
    }
    else if (settings.outputSentenceGSV == false)
    {
      if (getNMEASettings(UBX_NMEA_GSV, portType) != 0)
        response &= disableNMEAMessage(UBX_NMEA_GSV, portType);
    }

    if (settings.outputSentenceRMC == true)
    {
        if (getNMEASettings(UBX_NMEA_RMC, portType) != 1)
            response &= enableNMEAMessage(UBX_NMEA_RMC, portType);
    }
    else if (settings.outputSentenceRMC == false)
    {
      if (getNMEASettings(UBX_NMEA_RMC, portType) != 0)
        response &= disableNMEAMessage(UBX_NMEA_RMC, portType);
    }

    if (settings.outputSentenceGST == true)
    {
        if (getNMEASettings(UBX_NMEA_GST, portType) != 1)
            response &= enableNMEAMessage(UBX_NMEA_GST, portType);
    }
    else if (settings.outputSentenceGST == false)
    {
        if (getNMEASettings(UBX_NMEA_GST, portType) != 0)
            response &= disableNMEAMessage(UBX_NMEA_GST, portType);
    }

    return (response);
}


uint8_t ZED_F9P::getNMEASettings (uint8_t msgID, uint8_t portID)
{
    ubxPacket customCfg = {0, 0, 0, 0, 0, payloadCfg, 0, 0, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED};

    customCfg.cls = UBX_CLASS_CFG; // This is the message Class
    customCfg.id = UBX_CFG_MSG; // This is the message ID
    customCfg.len = 2;
    customCfg.startingSpot = 0; // Always set the startingSpot to zero (unless you really know what you are doing)

    uint16_t maxWait = 250; // Wait for up to 250ms (Serial may need a lot longer e.g. 1100)

    payloadCfg[0] = UBX_CLASS_NMEA;
    payloadCfg[1] = msgID;

    // Read the current setting. The results will be loaded into customCfg.
    if (sendCommand(&customCfg, maxWait) != SFE_UBLOX_STATUS_DATA_RECEIVED) // We are expecting data and an ACK
    {
        Serial.println(F("getNMEASettings failed!"));
        return (false);
    }

    return (payloadCfg[2 + portID]); //Return just the byte associated with this portID 
}


uint8_t ZED_F9P::getRTCMSettings(uint8_t msgID, uint8_t portID)
{
  ubxPacket customCfg = {0, 0, 0, 0, 0, payloadCfg, 0, 0, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED};

  customCfg.cls = UBX_CLASS_CFG; // This is the message Class
  customCfg.id = UBX_CFG_MSG; // This is the message ID
  customCfg.len = 2;
  customCfg.startingSpot = 0; // Always set the startingSpot to zero (unless you really know what you are doing)

  uint16_t maxWait = 250; // Wait for up to 250ms (Serial may need a lot longer e.g. 1100)

  payloadCfg[0] = UBX_RTCM_MSB;
  payloadCfg[1] = msgID;

  // Read the current setting. The results will be loaded into customCfg.
  if (sendCommand(&customCfg, maxWait) != SFE_UBLOX_STATUS_DATA_RECEIVED) // We are expecting data and an ACK
  {
    Serial.println(F("getRTCMSettings failed!"));
    return (false);
  }

  return (payloadCfg[2 + portID]); //Return just the byte associated with this portID
}


uint32_t ZED_F9P::getSerialRate(uint8_t portID)
{
  ubxPacket customCfg = {0, 0, 0, 0, 0, payloadCfg, 0, 0, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED};

  customCfg.cls = UBX_CLASS_CFG; // This is the message Class
  customCfg.id = UBX_CFG_PRT; // This is the message ID
  customCfg.len = 1;
  customCfg.startingSpot = 0; // Always set the startingSpot to zero (unless you really know what you are doing)

  uint16_t maxWait = 250; // Wait for up to 250ms (Serial may need a lot longer e.g. 1100)

  payloadCfg[0] = portID;

  // Read the current setting. The results will be loaded into customCfg.
  if (sendCommand(&customCfg, maxWait) != SFE_UBLOX_STATUS_DATA_RECEIVED) // We are expecting data and an ACK
  {
    Serial.println(F("getSerialRate failed!"));
    return (false);
  }
  return ((uint32_t)payloadCfg[10] << 16 | ((uint32_t)payloadCfg[9] << 8) | payloadCfg[8]);
}

