#ifndef _ZED_F9P_H_
#define _ZED_F9P_H_

#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_Ublox_Arduino_Library.h"
#include "PUBX_Parse.h"


#define deviceAddress 0x42
#define MAX_PAYLOAD_SIZE 256
#define UART0 0
#define UART1 1
#define UART2 2
#define OUTPUT_SETTING 14/*Byte that indicates output protocol setting in payload*/
#define INPUT_SETTING 12/*Byte that indicates input protocol setting in payload*/

class ZED_F9P: public SFE_UBLOX_GPS, public PUBX, public HardwareSerial
{
public:
    /**
     * @brief Constructor for ZED_f9P class
     * @param uart_nr name of UART
     * @param buff pointer to buffer
     * @param len length of buffer
     */
    ZED_F9P(int uart_nr, void * buff, uint8_t len);

    /**
     * @brief begin I2C and UART port of ESP32
     * @param baud baud speed for uart port
     * @retval none
     */
    void begin (unsigned long baud);

    /**
     * @brief configure Rover module
     * @param none
     * @retval true when successfully configure
     */
    bool config_GNSS ();

    /**
     * @brief get GNSS status
     * @param none
     * @retval state of Rover: no fix, 3D fix, 2D fix
     */
    //uint8_t get_GNSS_status ();

    int get_latitude();
    int get_longitude();
    

private:

    bool enableNMEASentences (uint8_t portType);
    bool getPortSetting (uint8_t portID);
    uint8_t getNMEASettings (uint8_t msgID, uint8_t portID);
    uint8_t settingPayload[MAX_PAYLOAD_SIZE];
    uint8_t getRTCMSettings(uint8_t msgID, uint8_t portID);
    uint32_t getSerialRate (uint8_t portID);

    typedef enum
    {
        COORD_TYPE_ECEF = 0,
        COORD_TYPE_GEOGRAPHIC,
    } coordinateType_e;
    
    struct struct_settings
    {
        int sizeOfSettings = 0; //sizeOfSettings **must** be the first entry and must be int
        uint8_t gnssMeasurementFrequency = 4; //Number of fixes per second
        bool printDebugMessages = false;
        bool enableSD = true;
        bool enableDisplay = true;
        bool zedOutputLogging = false;
        bool gnssRAWOutput = false;
        bool frequentFileAccessTimestamps = false;
        int maxLogTime_minutes = 60*10; //Default to 10 hours
        int observationSeconds = 60; //Default survey in time of 60 seconds
        float observationPositionAccuracy = 5.0; //Default survey in pos accy of 5m
        bool fixedBase = false; //Use survey-in by default
        bool fixedBaseCoordinateType = COORD_TYPE_ECEF;
        double fixedEcefX = 0.0;
        double fixedEcefY = 0.0;
        double fixedEcefZ = 0.0;
        double fixedLat = 0.0;
        double fixedLong = 0.0;
        double fixedAltitude = 0.0;
        uint32_t dataPortBaud = 115200; //Default to 115200bps
        uint32_t radioPortBaud = 57600; //Default to 57600bps to support connection to SiK1000 radios
        bool outputSentenceGGA = true;
        bool outputSentenceGSA = true;
        bool outputSentenceGSV = true;
        bool outputSentenceRMC = true;
        bool outputSentenceGST = true;
        bool enableSBAS = false; //Bug in ZED-F9P v1.13 firmware causes RTK LED to not light when RTK Floating with SBAS on.
        bool enableNtripServer = false;
        char casterHost[50] = "rtk2go.com"; //It's free...
        uint16_t casterPort = 2101;
        char mountPoint[50] = "bldr_dwntwn2";
        char mountPointPW[50] = "WR5wRo4H";
        char wifiSSID[50] = "TRex";
        char wifiPW[50] = "parachutes";  
    }settings;

};


#endif      /* _ZED_F9P_H_ */