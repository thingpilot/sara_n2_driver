/**
  * @file    SaraN2Driver.cpp
  * @version 0.1.0
  * @author  Adam Mitchell
  * @brief   C++ file of the NB-IoT driver module
  */

/** Includes
 */
#include "SaraN2Driver.h"


SaraN2::SaraN2(PinName txu, PinName rxu, PinName cts, PinName rst, PinName vint, PinName gpio, int baud) :
			   _cts(cts), _rst(rst, 1), _vint(vint), _gpio(gpio)
{
	_serial = new UARTSerial(txu, rxu, baud);
	_parser = new ATCmdParser(_serial);
	_parser->set_delimiter("\r\n");
	_parser->set_timeout(500);
}


SaraN2::~SaraN2()
{
	delete _serial;
	delete _parser;
}


int SaraN2::at()
{
	_parser->flush();

	_parser->send("AT");
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_AT;
	}

	return SaraN2::SARAN2_OK;
}