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


int SaraN2::select_profile(uint8_t profile)
{
	if(profile > NUMBER_OF_PROFILES) 
	{
		return SaraN2::INVALID_PROFILE;
	}

	_parser->flush();

	_parser->send("AT+UCOAP=3,\"%d\"", profile);
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_SELECT_PROFILE;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::load_profile(uint8_t profile)
{
	if(profile > NUMBER_OF_PROFILES) 
	{
		return SaraN2::INVALID_PROFILE;
	}

	_parser->flush();

	_parser->send("AT+UCOAP=5,\"%d\"", profile);
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_LOAD_PROFILE;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::save_profile(uint8_t profile)
{
	if(profile > NUMBER_OF_PROFILES) 
	{
		return SaraN2::INVALID_PROFILE;
	}

	_parser->flush();

	_parser->send("AT+UCOAP=6,\"%d\"", profile);
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_SAVE_PROFILE;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::set_profile_validity(uint8_t valid)
{
	if(valid > 1)
	{
		return SaraN2::VALUE_OUT_OF_BOUNDS;
	}

	_parser->flush();

	_parser->send("AT+UCOAP=4,\"%d\"", valid);
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_SET_PROFILE_VALIDITY;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::set_coap_ip_port(char *ipv4, uint16_t port)
{
	_parser->flush();

	_parser->send("AT+UCOAP=0,\"%s\",\"%d\"", ipv4, port);
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_SET_COAP_IP_PORT;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::set_coap_uri(char *uri)
{
	int uri_length = sizeof(uri) / sizeof(uri[0]);

	if(uri_length > 200)
	{
		return SaraN2::URI_TOO_LONG;
	}

	_parser->flush();

	_parser->send("AT+UCOAP=1,\"%s\"", uri);
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_SET_COAP_URI;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::pdu_header_add_uri_host()
{
	_parser->flush();

	_parser->send("AT+UCOAP=2,\"0\",\"1\"");
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_ADD_URI_HOST_PDU;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::pdu_header_add_uri_port()
{
	_parser->flush();

	_parser->send("AT+UCOAP=2,\"1\",\"1\"");
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_ADD_URI_PORT_PDU;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::pdu_header_add_uri_path()
{
	_parser->flush();

	_parser->send("AT+UCOAP=2,\"2\",\"1\"");
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_ADD_URI_PATH_PDU;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::pdu_header_add_uri_query()
{
	_parser->flush();

	_parser->send("AT+UCOAP=2,\"3\",\"1\"");
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_ADD_URI_QUERY_PDU;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::pdu_header_remove_uri_host()
{
	_parser->flush();

	_parser->send("AT+UCOAP=2,\"0\",\"0\"");
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_REMOVE_URI_HOST_PDU;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::pdu_header_remove_uri_port()
{
	_parser->flush();

	_parser->send("AT+UCOAP=2,\"1\",\"0\"");
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_REMOVE_URI_PORT_PDU;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::pdu_header_remove_uri_path()
{
	_parser->flush();

	_parser->send("AT+UCOAP=2,\"2\",\"0\"");
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_REMOVE_URI_PATH_PDU;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::pdu_header_remove_uri_query()
{
	_parser->flush();

	_parser->send("AT+UCOAP=2,\"3\",\"0\"");
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_REMOVE_URI_QUERY_PDU;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::select_coap_at_interface()
{
	_parser->flush();

	_parser->send("AT+USELCP=1");
	if(!_parser->recv("OK"))
	{
		return SaraN2::FAIL_SELECT_COAP_AT_INTERFACE;
	}

	return SaraN2::SARAN2_OK;
}


int SaraN2::coap_get()
{
	return SaraN2::SARAN2_OK;
}


int SaraN2::coap_delete()
{
	return SaraN2::SARAN2_OK;
}


int SaraN2::coap_put()
{
	return SaraN2::SARAN2_OK;
}


int SaraN2::coap_post()
{
	return SaraN2::SARAN2_OK;
}