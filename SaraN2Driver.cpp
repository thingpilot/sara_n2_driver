/**
  * @file    SaraN2Driver.cpp
  * @version 0.1.0
  * @author  Adam Mitchell
  * @brief   C++ file of the NB-IoT driver module
  */

/** Includes
 */
#include "SaraN2Driver.h"

/** Constructor for the SaraN2 class. Instantiates an ATCmdParser object
 *  on the heap for comms between microcontroller and modem
 * 
 * @param txu Pin connected to SaraN2 TXD (This is MCU TXU)
 * @param rxu Pin connected to SaraN2 RXD (This is MCU RXU)
 * @param cts Pin connected to SaraN2 CTS
 * @param rst Pin connected to SaraN2 RST
 * @param vint Pin conencted to SaraN2 VINT
 * @param gpio Pin connected to SaraN2 GPIO1
 * @param baud Baud rate for UART between MCU and SaraN2
 */  
SaraN2::SaraN2(PinName txu, PinName rxu, PinName cts, PinName rst, PinName vint, 
               PinName gpio, int baud) :
			   _cts(cts), _rst(rst, 1), _vint(vint), _gpio(gpio)
{
	_serial = new UARTSerial(txu, rxu, baud);
	_parser = new ATCmdParser(_serial);
	_parser->set_delimiter("\r\n");
	_parser->set_timeout(500);
}

/** Destructor for the SaraN2 class. Deletes the UARTSerial and ATCmdParser
 *  objects from the heap to release unused memory
 */  
SaraN2::~SaraN2()
{
	delete _serial;
	delete _parser;
}

/** Send "AT" command
 *
 * @return Indicates success or failure 
 */
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

/** Select CoAP profile number, between 0-3
 *
 * @param profile Use enumerated values COAP_PROFILE_x to select profile
 *                corresponding to that number
 * @return Indicates success or failure reason
 */
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

/** Restore CoAP profile from NVM, between 0-3
 *
 * @param profile Use enumerated values COAP_PROFILE_x to restore profile
 *                corresponding to that number
 * @return Indicates success or failure reason
 */
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

/** Store CoAP profile to NVM, between 0-3
 *
 * @param profile Use enumerated values COAP_PROFILE_x to store profile
 *                corresponding to that number
 * @return Indicates success or failure reason
 */
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

/** Set valid flag of profile to either valid or invalid. Only valid profiles
 *  can be used for CoAP operations.
 *
 * @param valid Use enumerated values PROFILE_VALID or PROFILE_INVALID 
 *              to set validity flag
 * @return Indicates success or failure reason
 */
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

/** Set destination IP address and CoAP port to which to send message
 *
 * @param *ipv4 Pointer to a byte array storing the IPv4 address of the 
 *              destination server as a string, for example:
 *              char ipv4[] = "168.134.102.18";  
 * @param port Destination server port
 * @return Indicates success or failure reason
 */
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

/** Set URI option in the PDU
 *
 * @param *uri Pointer to a byte array storing the URI, for example:
 *             char uri[] = "http://coap.me:5683/sink";
 * @return Indicates success or failure reason
 */
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

/** Add the URI host option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
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

/** Add the URI port option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
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

/** Add the URI path option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
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

/** Add the URI query option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
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

/** Remove the URI host option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
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

/** Remove the URI port option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
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

/** Remove the URI path option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
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

/** Remove the URI query option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
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

/** Select CoAP component for AT use. Because the Sara module's internal 
 *  CoAP library is shared with other components in the module, we must 
 *  explicity select the right CoAP component before use
 * 
 * @return Indicates success or failure reason
 */  
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