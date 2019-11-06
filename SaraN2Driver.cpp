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
	_smutex.lock();

	_parser->flush();

	_parser->send("AT");
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_AT;
	}

	_smutex.unlock();

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

	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=3,\"%d\"", profile);
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_SELECT_PROFILE;
	}

	_smutex.unlock();

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

	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=5,\"%d\"", profile);
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_LOAD_PROFILE;
	}

	_smutex.unlock();

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

	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=6,\"%d\"", profile);
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_SAVE_PROFILE;
	}

	_smutex.unlock();

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

	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=4,\"%d\"", valid);
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_SET_PROFILE_VALIDITY;
	}

	_smutex.unlock();

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
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=0,\"%s\",\"%d\"", ipv4, port);
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_SET_COAP_IP_PORT;
	}

	_smutex.unlock();

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

	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=1,\"%s\"", uri);
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_SET_COAP_URI;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Add the URI host option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
int SaraN2::pdu_header_add_uri_host()
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=2,\"0\",\"1\"");
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_ADD_URI_HOST_PDU;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Add the URI port option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
int SaraN2::pdu_header_add_uri_port()
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=2,\"1\",\"1\"");
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_ADD_URI_PORT_PDU;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Add the URI path option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
int SaraN2::pdu_header_add_uri_path()
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=2,\"2\",\"1\"");
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_ADD_URI_PATH_PDU;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Add the URI query option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
int SaraN2::pdu_header_add_uri_query()
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=2,\"3\",\"1\"");
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_ADD_URI_QUERY_PDU;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Remove the URI host option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
int SaraN2::pdu_header_remove_uri_host()
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=2,\"0\",\"0\"");
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_REMOVE_URI_HOST_PDU;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Remove the URI port option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
int SaraN2::pdu_header_remove_uri_port()
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=2,\"1\",\"0\"");
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_REMOVE_URI_PORT_PDU;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Remove the URI path option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
int SaraN2::pdu_header_remove_uri_path()
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=2,\"2\",\"0\"");
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_REMOVE_URI_PATH_PDU;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Remove the URI query option to the Protocol Data Unit (PDU) header
 *
 * @return Indicates success or failure reason
 */
int SaraN2::pdu_header_remove_uri_query()
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAP=2,\"3\",\"0\"");
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_REMOVE_URI_QUERY_PDU;
	}

	_smutex.unlock();

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
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+USELCP=1");
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_SELECT_COAP_AT_INTERFACE;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Parse response from CoAP server into recv_data
 * 
 * @param *recv_data Pointer to a byte array where the data
 *                   returned from the server will be stored
 * @param timeout_ms Timeout value for the parser in milliseconds
 * @return Indicates success or failure reason
 */
int SaraN2::parse_coap_response(char *recv_data, uint16_t timeout)
{
	_parser->set_timeout(timeout);

	int response_code = -1;
	int more_block = -1;

	if(!_parser->recv("+UCOAPCD:%d,\"(.*?)\",%d", &response_code, recv_data, &more_block))
	{
		return SaraN2::FAIL_PARSE_RESPONSE;
	}

	_parser->set_timeout(500);

	return SaraN2::SARAN2_OK;
}

/** Perform a GET request using CoAP and save the returned 
 *  data into recv_data
 * 
 * @param *recv_data Pointer to a byte array where the data 
 *                   returned from the server will be stored
 * @return Indicates success or failure reason
 */ 
int SaraN2::coap_get(char *recv_data)
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAPC=1");
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_START_GET_REQUEST;
	}

	if(parse_coap_response(recv_data) != SaraN2::SARAN2_OK)
	{
		_smutex.unlock();
		return SaraN2::FAIL_PARSE_RESPONSE;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Perform a DELETE request using CoAP and save the returned 
 *  data into recv_data
 * 
 * @param *recv_data Pointer to a byte array where the data 
 *                   returned from the server will be stored
 * @return Indicates success or failure reason
 */ 
int SaraN2::coap_delete(char *recv_data)
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAPC=2");
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_START_DELETE_REQUEST;
	}

	if(parse_coap_response(recv_data) != SaraN2::SARAN2_OK)
	{
		_smutex.unlock();
		return SaraN2::FAIL_PARSE_RESPONSE;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Perform a PUT request using CoAP and save the returned 
 *  data into recv_data
 * 
 * @param *send_data Pointer to a byte array containing the 
 *                   data to be sent to the server
 * @param *recv_data Pointer to a byte array where the data 
 *                   returned from the server will be stored
 * @param data_intenfier Integer value representing the data 
 *                       format type. Possible values are enumerated
 *                       in the header file, i.e. TEXT_PLAIN
 * @return Indicates success or failure reason
 */ 
int SaraN2::coap_put(char *send_data, char *recv_data, int data_indentifier)
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAPC=3,\"%s\",%i", send_data, data_indentifier);
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_START_PUT_REQUEST;
	}

	if(parse_coap_response(recv_data) != SaraN2::SARAN2_OK)
	{
		_smutex.unlock();
		return SaraN2::FAIL_PARSE_RESPONSE;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Perform a POST request using CoAP and save the returned 
 *  data into recv_data
 * 
 * @param *send_data Pointer to a byte array containing the 
 *                   data to be sent to the server
 * @param *recv_data Pointer to a byte array where the data 
 *                   returned from the server will be stored
 * @param data_intenfier Integer value representing the data 
 *                       format type. Possible values are enumerated
 *                       in the header file, i.e. TEXT_PLAIN
 * @return Indicates success or failure reason
 */ 
int SaraN2::coap_post(char *send_data, char *recv_data, int data_indentifier)
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+UCOAPC=4,\"%s\",%i", send_data, data_indentifier);
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_START_POST_REQUEST;
	}

	if(parse_coap_response(recv_data) != SaraN2::SARAN2_OK)
	{
		_smutex.unlock();
		return SaraN2::FAIL_PARSE_RESPONSE;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}


/** Reboots the module. After receiving the 'REBOOTING' response, no further
 *  AT commands will be processed until the module has successfully power on
 *
 * @return Indicates success or failure reason
 */
int SaraN2::reboot_module()
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+NRB");
	if(!_parser->recv("REBOOTING"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_REBOOT;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Configure customisable aspects of the UE given the functions and values
 *  available in the enumerated list of AT+NCONFIG functions and values
 * 
 * @param function Enumerated value of AT+NCONFIG function to perform
 * @param value Enumerated value of AT+NCONFIG value to use in function
 * @return Indicates success or failure reason 
 */ 
int SaraN2::configure_ue(uint8_t function, uint8_t value)
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+NCONFIG=\"%s\",\"%s\"", config_functions[function], config_values[value]);
	if(!_parser->recv("OK"))
	{
		_smutex.unlock();
		return SaraN2::FAIL_CONFIGURE_UE;
	}

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}

/** Return operation stats, of a given type, of the module
 * 
 * @param type Enumerated type of the AT+NUESTATS types to query
 * @return Indicates success or failure reason
 */
int SaraN2::nuestats(uint8_t type)
{
	_smutex.lock();

	_parser->flush();

	_parser->send("AT+NUESTATS=\"%s\"", nuestats_types[type]);

	/* Do stuff here to parse NUESTATS output */

	_smutex.unlock();

	return SaraN2::SARAN2_OK;
}