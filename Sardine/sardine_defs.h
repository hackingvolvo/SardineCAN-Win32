#ifndef _SARDINE_DEFS
#define _SARDINE_DEFS
#include "j2534_v0404.h"


// REMINDER: SAE  J2534-2 document:  http://wenku.baidu.com/view/2f3378c30c22590102029d31.html###
//			 SAE J2534-1 documnets : http://wenku.baidu.com/view/ac89e723dd36a32d7375813d

#define SARDINE_DEVICE_ID 0xcafebabe
#define SARDINE_DEFAULT_CAN_BAUD_RATE 125000
#define COMM_INIT_TIMEOUT 2000	// we wait 2 seconds for Comm thread to initialize Arduino in Sardine::Connect (though it normally should have been initialized already straight after launching the DLL)

#define SARDINE_FIRMWARE_VERSION "00.20"	// FIXME: Get the Sardine CAN version from Arduino
#define SARDINE_DLL_VERSION	"00.20"
#define SARDINE_J2534_API_VERSION "04.04"

#define MAX_J2534_MESSAGES 10

#define IGNORE_SILENTLY_UNIMPLEMENTED_FEATURES
//#define ENFORCE_PROTOCOL_IDS_IN_MSGS  // seen atleast once occasion where VIDA sends msgs with protocol id 5997 when protocol is ISO 15765 (id 6)

// flow status (flow control message)
#define ISO15765_FS_CTS			0	// clear to send
#define ISO15765_FS_WAIT		1	// wait
#define ISO15765_FS_OVERFLOW	2	// overflow/abort

// ISO15765 message types in PCI high nibble
#define ISO15765_PCI_SINGLE_FRAME		0x00
#define ISO15765_PCI_FIRST_FRAME		0x10
#define ISO15765_PCI_CONSECUTIVE_FRAME	0x20
#define ISO15765_PCI_FLOW_CONTROL_FRAME	0x30



/*************/
/* Error IDs */
/*************/
/*
#define STATUS_NOERROR						0x00	// Function completed successfully.
#define ERR_NOT_SUPPORTED					0x01	// Function option is not supported.
#define ERR_INVALID_CHANNEL_ID				0x02	// Channel Identifier or handle is not recognized.
#define ERR_INVALID_PROTOCOL_ID				0x03	// Protocol Identifier is not recognized.
#define ERR_NULL_PARAMETER					0x04	// NULL pointer presented as a function parameter, NULL is an invalid address.
#define ERR_NULLPARAMETER					0x04	// NULL pointer presented as a function parameter, NULL is an invalid address.
#define ERR_INVALID_IOCTL_VALUE				0x05	// Ioctl GET_CONFIG/SET_CONFIG parameter value is not recognized.
#define ERR_INVALID_FLAGS					0x06	// Flags bit field(s) contain(s) an invalid value.
#define ERR_FAILED							0x07	// Unspecified error, use PassThruGetLastError for obtaining error text string.
#define ERR_DEVICE_NOT_CONNECTED			0x08	// PassThru device is not connected to the PC.
#define ERR_TIMEOUT							0x09	// Timeout violation. PassThru device is unable to read specified number of messages from the vehicle network. The actual number of messages returned is in NumMsgs.
#define ERR_INVALID_MSG						0x0A	// Message contained a min/max length, ExtraData support or J1850PWM specific source address conflict violation.
#define ERR_INVALID_TIME_INTERVAL			0x0B	// The time interval value is outside the specified range.
#define ERR_EXCEEDED_LIMIT					0x0C	// The limit (ten) of filter/periodic messages has been exceeded for the protocol associated the communications channel.
#define ERR_INVALID_MSG_ID					0x0D	// The message identifier or handle is not recognized.
//#define ERR_INVALID_ERROR_ID				0x0E 
#define	ERR_DEVICE_IN_USE					0x0E	// The specified PassThru device is already in use.
#define ERR_INVALID_IOCTL_ID				0x0F	// Ioctl identifier is not recognized.
#define ERR_BUFFER_EMPTY					0x10	// PassThru device could not read any messages from the vehicle network.
#define ERR_BUFFER_FULL						0x11	// PassThru device could not queue any more transmit messages destined for the vehicle network.
#define ERR_BUFFER_OVERFLOW					0x12	// PassThru device experienced a buffer overflow and receive messages were lost.
#define ERR_PIN_INVALID						0x13	// Unknown pin number specified for the J1962 connector, or resource is already in use.
#define ERR_CHANNEL_IN_USE					0x14	// 	An existing communications channel is currently using the specified network protocol.
#define ERR_MSG_PROTOCOL_ID					0x15 	// The specified protocol type within the message structure is different from the protocol associated with the communications channel when it was opened.
#define ERR_INVALID_FILTER_ID				0x16 	// Filter identifier is not recognized.
#define	ERR_NO_FLOW_CONTROL					0x17 	// No ISO15765 flow control filter is set, or no filter matches the header of an outgoing message.
#define ERR_NOT_UNIQUE 						0x18	// An existing filter already matches this header or node identifier.
#define ERR_INVALID_BAUDRATE				0x19 	// Unable to honor requested Baud rate within required tolerances.
#define ERR_INVALID_DEVICE_ID				0x1A 	// PassThru device identifier is not recognized.
*/

static const struct J2534_error_msg { 
	int err_id;
	char * msg;
} J2534_error_msgs[] = {    
	{ STATUS_NOERROR				,"Function completed successfully." },
	{ ERR_NOT_SUPPORTED				,"Function option is not supported." },
	{ ERR_INVALID_CHANNEL_ID		,"Channel Identifier or handle is not recognized." },
	{ ERR_INVALID_PROTOCOL_ID		,"Protocol Identifier is not recognized."},
	{ ERR_NULL_PARAMETER			,"pointer presented as a function parameter, NULL is an invalid address."},
	{ ERR_NULLPARAMETER				,"NULL pointer presented as a function parameter, NULL is an invalid address."},
	{ ERR_INVALID_IOCTL_VALUE		,"Ioctl GET_CONFIG/SET_CONFIG parameter value is not recognized."},
	{ ERR_INVALID_FLAGS				,"Flags bit field(s) contain(s) an invalid value."},
	{ ERR_FAILED					,"Unspecified error, use PassThruGetLastError for obtaining error text string."},
	{ ERR_DEVICE_NOT_CONNECTED		,"PassThru device is not connected to the PC."},
	{ ERR_TIMEOUT					,"Timeout violation. Requested number of messages not read."}, // PassThru device is unable to read specified number of messages from the vehicle network. The actual number of messages returned is in NumMsgs."},
	{ ERR_INVALID_MSG				,"Invalid message"}, // Message contained a min/max length, ExtraData support or J1850PWM specific source address conflict violation."},
	{ ERR_INVALID_TIME_INTERVAL		,"The time interval value is outside the specified range."},
	{ ERR_EXCEEDED_LIMIT			,"The limit (ten) of filter/periodic messages has been exceeded."}, // for the protocol associated the communications channel."},
	{ ERR_INVALID_MSG_ID			,"The message identifier or handle is not recognized."},
	{ ERR_DEVICE_IN_USE				,"The specified PassThru device is already in use."},
	{ ERR_INVALID_IOCTL_ID			,"Ioctl identifier is not recognized."},
	{ ERR_BUFFER_EMPTY				,"PassThru device could not read any messages from the vehicle network."},
	{ ERR_BUFFER_FULL				,"PassThru device could not queue any more transmit messages."},
	{ ERR_BUFFER_OVERFLOW			,"PassThru device experienced a buffer overflow and receive messages were lost."},
	{ ERR_PIN_INVALID				,"Unknown pin specified for the J1962 connector, or resource is already in use."},
	{ ERR_CHANNEL_IN_USE			,"Existing communications channel is currently using the specified protocol."},
	{ ERR_MSG_PROTOCOL_ID			,"Protocol type specified in message differs from the configured protocol type."}, //The specified protocol type within the message structure is different from the protocol associated with the communications channel when it was opened."},
	{ ERR_INVALID_FILTER_ID			,"Filter identifier is not recognized."},
	{ ERR_NO_FLOW_CONTROL			,"No flow control filter is set, or no filter matches an outgoing message."},
	{ ERR_NOT_UNIQUE 				,"An existing filter already matches this header or node identifier."},
	{ ERR_INVALID_BAUDRATE			,"Unable to honor requested Baud rate within required tolerances."},
	{ ERR_INVALID_DEVICE_ID			,"PassThru device identifier is not recognized."},
	{ -1, NULL }
};

 
//typedef struct {
//	unsigned long ProtocolID; /* vehicle network protocol */
//	unsigned long RxStatus; /* receive message status */
//	unsigned long TxFlags; /* transmit message flags */
//	unsigned long Timestamp; /* receive message timestamp(in microseconds) */
//	unsigned long DataSize; /* byte size of message payload in the Data array */
//	unsigned long ExtraDataIndex; /* start of extra data(i.e. CRC, checksum, etc) in Data array */
//	unsigned char Data[4128]; /* message payload or data */
//} PASSTHRU_MSG;

#endif