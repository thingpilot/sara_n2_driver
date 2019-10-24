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
			SARAN2_OK 				  	  = 0,
			FAIL_AT             	  	  = 1,
			FAIL_SELECT_PROFILE       	  = 2,
			INVALID_PROFILE           	  = 3,
			FAIL_LOAD_PROFILE         	  = 4,
			FAIL_SAVE_PROFILE         	  = 5,
			FAIL_SET_PROFILE_VALIDITY 	  = 6,
			VALUE_OUT_OF_BOUNDS       	  = 7,
			FAIL_SET_COAP_IP_PORT     	  = 8,
			FAIL_SET_COAP_URI         	  = 9,
			URI_TOO_LONG              	  = 10,
			FAIL_ADD_URI_HOST_PDU     	  = 11,
			FAIL_ADD_URI_PORT_PDU     	  = 12,
			FAIL_ADD_URI_PATH_PDU     	  = 13,
			FAIL_ADD_URI_QUERY_PDU    	  = 14,
			FAIL_REMOVE_URI_HOST_PDU  	  = 15,
			FAIL_REMOVE_URI_PORT_PDU  	  = 16,
			FAIL_REMOVE_URI_PATH_PDU      = 17,
			FAIL_REMOVE_URI_QUERY_PDU     = 18,
			FAIL_SELECT_COAP_AT_INTERFACE = 19
		};

		enum
		{
			COAP_PROFILE_0 = 0,
			COAP_PROFILE_1 = 1,
			COAP_PROFILE_2 = 2,
			COAP_PROFILE_3 = 3
		};

		enum
		{
			PROFILE_INVALID = 0,
			PROFILE_VALID = 1
		};

		enum
		{
			TEXT_PLAIN        = 0,
			APPLICATION_LINK  = 1,
			APPLICATION_XML   = 2,
			APPLICATION_OCTET = 3,
			APPLICATION_RDF   = 4,
			APPLICATION_EXI   = 5,
			APPLICATION_JSON  = 6,
			APPLICATION_CBOR  = 7
		};

		SaraN2(PinName txu, PinName rxu, PinName cts, PinName rst, PinName vint, 
			   PinName gpio, int baud = 57600);

		~SaraN2();

		/** Send "AT" command
         *
         * @return Indicates success or failure 
         */
		int at();

		/** Select CoAP profile number, between 0-3
         *
		 * @param profile Use enumerated values COAP_PROFILE_x to select profile
		 *                corresponding to that number
         * @return Indicates success or failure reason
         */
		int select_profile(uint8_t profile);

		/** Restore CoAP profile from NVM, between 0-3
         *
		 * @param profile Use enumerated values COAP_PROFILE_x to restore profile
		 *                corresponding to that number
         * @return Indicates success or failure reason
         */
		int load_profile(uint8_t profile);

		/** Store CoAP profile to NVM, between 0-3
         *
		 * @param profile Use enumerated values COAP_PROFILE_x to store profile
		 *                corresponding to that number
         * @return Indicates success or failure reason
         */
		int save_profile(uint8_t profile);

		int set_profile_validity(uint8_t valid);

		int set_coap_ip_port(char *ipv4, uint16_t port);

		int set_coap_uri(char *uri);

		int pdu_header_add_uri_host();

		int pdu_header_add_uri_port();

		int pdu_header_add_uri_path();

		int pdu_header_add_uri_query();

		int pdu_header_remove_uri_host();

		int pdu_header_remove_uri_port();

		int pdu_header_remove_uri_path();

		int pdu_header_remove_uri_query();

		int select_coap_at_interface();

		int coap_get();

		int coap_delete();

		int coap_put();

		int coap_post();

	private:

		DigitalIn  _cts;
		DigitalOut _rst;
		DigitalIn  _vint;
		DigitalIn  _gpio;

		UARTSerial  *_serial;
        ATCmdParser *_parser;
};

