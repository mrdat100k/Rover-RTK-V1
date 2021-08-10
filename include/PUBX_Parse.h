#ifndef _PUBX_PARSE_H
#define _PUBX_PARSE_H

#include <Arduino.h>



class PUBX {
public:
	/*Constructor*/
	PUBX(void* buffer, uint8_t len);

	/**
	 * @brief skip field which don't use
	 * @param s pointer to first position of that field
	 * @retval pointer to first position of the next field
	 */
    static const char* skipField (const char* s);

	/**
	 * @brief parse string to unsigned int number
	 * @param s pointer to first position of that field
	 * @param len length of that number you want to get from that field
	 * @retval parsed field
	 */
    static unsigned int parseUnsignedInt(const char* s, uint8_t len);

	/**
	 * @brief parse string to float number 
	 * @param s pointer to first position of that field
	 * @param log10Multiplier the number of character you want in fraction
	 * @param eptr pointer to the next field
	 * @retval parsed field
	 */
    static long parseFloat(const char* s, uint8_t log10Multiplier,
						   const char** eptr = nullptr);

	/**
	 * @brief parse longitude/latitude with degree minute format 
	 * @param s pointer to first position of that field
	 * @param degWidth width of degree
	 * @param eptr pointer to the next field
	 * @retval 
	 */
	static long parseDegreeMinute(const char* s, uint8_t degWidth,
								  const char** eptr = nullptr);

	/**
	 * @brief get the field
	 * @param s pointer to first position of that field
	 * @param result field after parse
	 * @retval location of the next field
	 */
	static const char* parseField(const char* s, char *result = nullptr,
								  int len = 0);

	/**
	 * @brief generate CRC checksum
	 * @param s pointer to first position of message
	 * @param checksum checksum result
	 * @retval pointer to checksum field of message
	 */							  
	static const char* generateChecksum(const char* s, char* checksum);

	/**
	 * @brief test checksum of message
	 * @param s pointer to first position of message
	 * @retval true when checksum is true
	 */
	static bool testChecksum(const char* s);


	/**
	 * @brief init value for all field
	 * @param none
	 * @retval node
	 */
	void clear (void);

	/**
	 * @brief set buffer for object
	 * @param buf pointer to buffer
	 * @param len length of buffer
	 * @retval none
	 */
	void setBuffer (void* buf, uint8_t len);

	/****************************************************************/

	/*The following function is for user to get all value of message*/

	uint8_t pubx_get_num_satellite (void) const {
		return _numSvs;
	}

	long pubx_get_longitude (void) const
	{
		return _longitude;
	}

	long pubx_get_latitude (void) const
	{
		return _latitude;
	}

	uint8_t pubx_get_horizontal_accuracy (void) const
	{
		return _hAcc;
	}

	uint8_t pubx_get_vertical_accuracy (void) const
	{
		return _vAcc;
	}

	uint8_t pubx_get_hour (void) const
	{
		return _hour;
	}

	uint8_t pubx_get_second (void) const
	{
		return _second;
	}

	uint8_t pubx_get_minute (void) const
	{
		return _minute;
	}

	uint8_t pubx_get_hundredth (void) const
	{
		return _hundredths;
	}

	long pubx_get_altitude (void) const
	{
		return _altRef;
	}
	uint8_t pubx_get_posMode (void) const
	{
		return _posMode;
	}
	bool processData (char c);

	bool processPOS (const char* s);

	


protected:
	static inline bool isEndOfFields(char c)
	{
		return c == '*' || c == '\0' || c == '\r' || c == '\n';
	}

	const char* parseTime(const char* s);
	
	const char* parseDate(const char* s);

    bool processPUBX(const char* s);

    bool processGNS(const char* s);

    bool processGSA(const char* s);

    bool processGLL(const char* s);

    bool processGGA(const char *s);

    bool processRMC(const char* s);



protected:
	uint8_t _bufferLen;
	char* _buffer;
	char *_ptr;
	char _navSystem;

	char _messageID[6];
	char _posMode;
	unsigned int svid[12];

	bool _isValid;
	bool _altitudeValid;
	long _latitude, _longitude, _altitude;
	long _altRef;
	long _navStat, _course,_speed;
	uint16_t _year;
	uint8_t _month, _day, _hour, _minute, _second, _hundredths;
	uint8_t _pdop;
	uint8_t _hdop;
	uint8_t _vdop;
	uint8_t _tdop;
	uint8_t _numSvs;
	uint8_t _hAcc;
	uint8_t _vAcc;
};

#endif
