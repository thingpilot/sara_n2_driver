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
}


SaraN2::~SaraN2()
{
	delete _serial;
	delete _parser;
}