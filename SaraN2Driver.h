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


class SaraN2
{

	public:

		enum
		{
			SARAN2_OK = 0
		};

		SaraN2(PinName txu, PinName rxu, PinName cts, PinName rst, PinName vint, PinName gpio, int baud = 57600);

		~SaraN2();

	private:

		DigitalIn  _cts;
		DigitalOut _rst;
		DigitalIn  _vint;
		DigitalIn  _gpio;

		UARTSerial  *_serial;
        ATCmdParser *_parser;
};

