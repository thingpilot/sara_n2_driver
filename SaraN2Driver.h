/**
  * @file    SaraN2Driver.h
  * @version 0.1.0
  * @author  Adam Mitchell
  * @brief   Header file of the  NB-IoT driver module
  */

/** Define to prevent recursive inclusion
 */
#pragma once

/** Includes 
 */
#include <mbed.h>
#include "board.h"

/** Module-specific #defines
 */
#define NUMBER_OF_PROFILES 3 

/*
AT+UCOAP=0,"165.22.122.212","5683"      set destination ip/port
AT+UCOAP=1,"coap://165.22.122.212:5683/uri"       set coap uri
AT+UCOAP=2,"2","1"       configure PDU to include uri path
AT+UCOAP=4,"1"      set profile as valid
AT+USELCP=1       turn on CoAP AT command mode
AT+UCOAPC=1     do GET request
*/

class SaraN2
{

	public:

		enum
		{
			SARAN2_OK 				  = 0,
			FAIL_AT             	  = 1,
			FAIL_SELECT_PROFILE       = 2,
			INVALID_PROFILE           = 3,
			FAIL_LOAD_PROFILE         = 4,
			FAIL_SAVE_PROFILE         = 5,
			FAIL_SET_PROFILE_VALIDITY = 6,
			VALUE_OUT_OF_BOUNDS       = 7
		};

		SaraN2(PinName txu, PinName rxu, PinName cts, PinName rst, PinName vint, PinName gpio, int baud = 57600);

		~SaraN2();

		int at();

		int select_profile(uint8_t profile);

		int load_profile(uint8_t profile);

		int save_profile(uint8_t profile);

		int set_profile_validity(uint8_t valid);

	private:

		DigitalIn  _cts;
		DigitalOut _rst;
		DigitalIn  _vint;
		DigitalIn  _gpio;

		UARTSerial  *_serial;
        ATCmdParser *_parser;
};

