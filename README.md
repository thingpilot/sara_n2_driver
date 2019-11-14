## Sara N2xx Driver Release Notes
**v0.3.0** *14/11/2019*

 - Query RSRP and RSRQ via AT+CSQ
 - Increased default value for CoAP response timeout to 20 seconds. Future updates will scale this value dynamically dependent on signal quality and network conditions
 - Get current module PSM status
 - Explicitly set CEREG URC value when querying

**v0.2.0** *12/11/2019*

 - Perform CoAP GET, DELETE, PUT and POST requests and return payload to application
 - AT-triggered module reboot and monitor for power-cycle complete
 - Enable/Disable module PSM and query PSM status
 - Set T3412 and T3324 timers
 - Query T3412 and T3324 timers and return the result as either binary string or enumerated value and multiples
 - Manually trigger GPRS attach/detach
 - Toggle RF circuitry on/off
 - Manually trigger network registration/deregistration

**v0.1.0** *24/10/2019*

 - Ability to configure CoAP profiles, including modification of PDU header
 - Software-triggered module reboot 
 - Placeholder functions for CoAP comms functions (GET, DELETE, PUT, POST)
 - AT+NCONFIG functionality, i.e. enabling AUTOCONNECT or CELL_RESELECTION
 - Structure in place to complete AT+NUESTATS functionality
