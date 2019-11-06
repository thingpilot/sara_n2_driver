/**
  * @file    SaraN2Driver.h
  * @version 0.1.0
  * @author  Adam Mitchell
  * @brief   Header file of the NB-IoT driver module
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

/** Base class for the SaraN2xx series of NB-IoT modules
 */ 
class SaraN2
{

	public:

		/** Function return codes
		 */
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
			FAIL_SELECT_COAP_AT_INTERFACE = 19,
			FAIL_REBOOT                   = 20,
			FAIL_CONFIGURE_UE             = 21,
			FAIL_START_GET_REQUEST        = 22,
			FAIL_START_DELETE_REQUEST     = 23,
			FAIL_START_PUT_REQUEST        = 24,
			FAIL_START_POST_REQUEST       = 25,
			FAIL_PARSE_RESPONSE           = 26
		};

		/** List of available CoAP profiles
		 */ 
		enum
		{
			COAP_PROFILE_0 = 0,
			COAP_PROFILE_1 = 1,
			COAP_PROFILE_2 = 2,
			COAP_PROFILE_3 = 3
		};

		/** Enumerated 'valid' flags
		 */ 
		enum
		{
			PROFILE_INVALID = 0,
			PROFILE_VALID = 1
		};

		/** Enumerated data format types
		 */
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

		/** Enumerated list of AT+NCONFIG functions
		 */
		enum
		{
			AUTOCONNECT        = 0,
			SCRAMBLING         = 1,
			SI_AVOID           = 2,
			COMBINE_ATTACH     = 3,
			CELL_RESELECTION   = 4,
			ENABLE_BIP         = 5,
			NAS_SIM_PSM_ENABLE = 6
		};
		
		/** Enumerated list of AT+NCONFIG values
		 */
		enum
		{
			TRUE  = 0,
			FALSE = 1
		};

		/** Enumerated list of AT+NUESTATS types
		 */
		enum
		{
			RADIO   = 0,
			CELL    = 1,
			BLER    = 2,
			APPSMEM = 3,
			THP     = 4
		};

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
		SaraN2(PinName txu, PinName rxu, PinName cts, PinName rst, PinName vint, 
			   PinName gpio, int baud = 57600);

		/** Destructor for the SaraN2 class. Deletes the UARTSerial and ATCmdParser
		 *  objects from the heap to release unused memory
		 */  
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

		/** Set valid flag of profile to either valid or invalid. Only valid profiles
		 *  can be used for CoAP operations.
         *
		 * @param valid Use enumerated values PROFILE_VALID or PROFILE_INVALID 
		 *              to set validity flag
         * @return Indicates success or failure reason
         */
		int set_profile_validity(uint8_t valid);

		/** Set destination IP address and CoAP port to which to send message
         *
		 * @param *ipv4 Pointer to a byte array storing the IPv4 address of the 
		 *              destination server as a string, for example:
		 *              char ipv4[] = "168.134.102.18";  
		 * @param port Destination server port
         * @return Indicates success or failure reason
         */
		int set_coap_ip_port(char *ipv4, uint16_t port);

		/** Set URI option in the PDU
         *
		 * @param *uri Pointer to a byte array storing the URI, for example:
		 *             char uri[] = "http://coap.me:5683/sink";
         * @return Indicates success or failure reason
         */
		int set_coap_uri(char *uri);

		/** Add the URI host option to the Protocol Data Unit (PDU) header
         *
         * @return Indicates success or failure reason
         */
		int pdu_header_add_uri_host();

		/** Add the URI port option to the Protocol Data Unit (PDU) header
         *
         * @return Indicates success or failure reason
         */
		int pdu_header_add_uri_port();

		/** Add the URI path option to the Protocol Data Unit (PDU) header
         *
         * @return Indicates success or failure reason
         */
		int pdu_header_add_uri_path();

		/** Add the URI query option to the Protocol Data Unit (PDU) header
         *
         * @return Indicates success or failure reason
         */
		int pdu_header_add_uri_query();

		/** Remove the URI host option to the Protocol Data Unit (PDU) header
         *
         * @return Indicates success or failure reason
         */
		int pdu_header_remove_uri_host();

		/** Remove the URI port option to the Protocol Data Unit (PDU) header
         *
         * @return Indicates success or failure reason
         */
		int pdu_header_remove_uri_port();

		/** Remove the URI path option to the Protocol Data Unit (PDU) header
         *
         * @return Indicates success or failure reason
         */
		int pdu_header_remove_uri_path();

		/** Remove the URI query option to the Protocol Data Unit (PDU) header
         *
         * @return Indicates success or failure reason
         */
		int pdu_header_remove_uri_query();

		/** Select CoAP component for AT use. Because the Sara module's internal 
		 *  CoAP library is shared with other components in the module, we must 
		 *  explicity select the right CoAP component before use
		 * 
		 * @return Indicates success or failure reason
		 */  
		int select_coap_at_interface();

		/** Parse response from CoAP server into recv_data
		 * 
		 * @param *recv_data Pointer to a byte array where the data
		 *                   returned from the server will be stored
		 * @param timeout_ms Timeout value for the parser in milliseconds
		 * @return Indicates success or failure reason
		 */
		int parse_coap_response(char *recv_data, uint16_t timeout_ms = 10000);

		/** Perform a GET request using CoAP and save the returned 
		 *  data into recv_data
		 * 
		 * @param *recv_data Pointer to a byte array where the data 
		 *                   returned from the server will be stored
		 * @return Indicates success or failure reason
		 */ 
		int coap_get(char *recv_data);

		/** Perform a DELETE request using CoAP and save the returned 
		 *  data into recv_data
		 * 
		 * @param *recv_data Pointer to a byte array where the data 
		 *                   returned from the server will be stored
		 * @return Indicates success or failure reason
		 */ 
		int coap_delete(char *recv_data);

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
		int coap_put(char *send_data, char *recv_data, int data_indentifier);

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
		int coap_post(char *send_data, char *recv_data, int data_indentifier);

		/** Reboots the module. After receiving the 'REBOOTING' response, no further
		 *  AT commands will be processed until the module has successfully power on
		 *
		 * @return Indicates success or failure reason
		 */
		int reboot_module();

		/** Configure customisable aspects of the UE given the functions and values
		 *  available in the enumerated list of AT+NCONFIG functions and values
		 * 
		 * @param function Enumerated value of AT+NCONFIG function to perform
		 * @param value Enumerated value of AT+NCONFIG value to use in function
		 * @return Indicates success or failure reason 
		 */ 
		int configure_ue(uint8_t function, uint8_t value);

		/** Return operation stats, of a given type, of the module
		 * 
		 * @param type Enumerated type of the AT+NUESTATS types to query
		 * @return Indicates success or failure reason
		 */
		int nuestats(uint8_t type);

	private:

		/** Potential AT+CONFIG function arguments, to be accessed using the enumerated
		 *  value that corresponds to the index of the function you wish to use, i.e:
		 *  config_functions[AUTOCONNECT];
		 */
		const char *config_functions[7] = { "AUTOCONNECT", 
                   "CR_0354_0338_SCRAMBLING", "CR_0859_SI_AVOID", "COMBINE_ATTACH",
				   "CELL_RESELECTION", "ENABLE_BIP", "NAS_SIM_POWER_SAVING_ENABLE" };

		/** Potential AT+NCONFIG value arguments, to be access using the enumerated
		 *  value that corresponds to the index of the value you wish to use, i.e:
		 *  config_values[TRUE];
		 */
		const char *config_values[2] = { "TRUE", "FALSE" };

		/** Potential AT+NUESTATS type arguments, to be access using the enumerated
		 *  type that corresponds to the index of the type you wish to use, i.e:
		 *  nuestats_types[RADIO];
		 */
		const char *nuestats_types[5] = { "RADIO", "CELL", "BLER", "APPSMEM", "THP" };

		DigitalIn  _cts;
		DigitalOut _rst;
		DigitalIn  _vint;
		DigitalIn  _gpio;

		UARTSerial  *_serial;
        ATCmdParser *_parser;
};

