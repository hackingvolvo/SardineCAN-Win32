/*
**
** Copyright (C) 2009 Drew Technologies Inc.
** Author: Joey Oravec <joravec@drewtech.com>
**
** This library is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation, either version 3 of the License, or (at
** your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, <http://www.gnu.org/licenses/>.
**
*/


#pragma once

/**************************/
/* ProtocolID definitions */
/**************************/

#define J1850VPW					1
#define J1850PWM					2
#define ISO9141						3
#define ISO14230					4
#define CAN							5
#define ISO15765					6
#define SCI_A_ENGINE				7
#define SCI_A_TRANS					8
#define SCI_B_ENGINE				9
#define SCI_B_TRANS					10

// J2534-2 Pin Switched ProtocolIDs

#define J1850VPW_PS					0x8000
#define J1850PWM_PS					0x8001
#define ISO9141_PS					0x8002
#define ISO14230_PS					0x8003
#define CAN_PS						0x8004
#define ISO15765_PS					0x8005
#define J2610_PS					0x8006
#define SW_ISO15765_PS				0x8007
#define SW_CAN_PS					0x8008
#define GM_UART_PS					0x8009
#define CAN_XON_XOFF_PS				0x800A
#define ANALOG_IN_1					0x800B
#define ANALOG_IN_2					0x800C
#define ANALOG_IN_3					0x800D
#define ANALOG_IN_4					0x800E
#define ANALOG_IN_5					0x800F
#define ANALOG_IN_6					0x8010
#define ANALOG_IN_7					0x8011
#define ANALOG_IN_8					0x8012
#define ANALOG_IN_9					0x8013
#define ANALOG_IN_10				0x8014
#define ANALOG_IN_11				0x8015
#define ANALOG_IN_12				0x8016
#define ANALOG_IN_13				0x8017
#define ANALOG_IN_14				0x8018
#define ANALOG_IN_15				0x8019
#define ANALOG_IN_16				0x801A
#define ANALOG_IN_17				0x801B
#define ANALOG_IN_18				0x801C
#define ANALOG_IN_19				0x801D
#define ANALOG_IN_20				0x801E
#define ANALOG_IN_21				0x801F
#define ANALOG_IN_22				0x8020
#define ANALOG_IN_23				0x8021
#define ANALOG_IN_24				0x8022
#define ANALOG_IN_25				0x8023
#define ANALOG_IN_26				0x8024
#define ANALOG_IN_27				0x8025
#define ANALOG_IN_28				0x8026
#define ANALOG_IN_29				0x8027
#define ANALOG_IN_30				0x8028
#define ANALOG_IN_31				0x8029
#define ANALOG_IN_32				0x802A

/*************/
/* IOCTL IDs */
/*************/

#define GET_CONFIG						0x01
#define SET_CONFIG						0x02
#define READ_VBATT						0x03
#define FIVE_BAUD_INIT					0x04
#define FAST_INIT						0x05
// unused								0x06
#define CLEAR_TX_BUFFER					0x07
#define CLEAR_RX_BUFFER					0x08
#define CLEAR_PERIODIC_MSGS				0x09
#define CLEAR_MSG_FILTERS				0x0A
#define CLEAR_FUNCT_MSG_LOOKUP_TABLE			0x0B
#define ADD_TO_FUNCT_MSG_LOOKUP_TABLE			0x0C
#define DELETE_FROM_FUNCT_MSG_LOOKUP_TABLE		0x0D
#define READ_PROG_VOLTAGE				0x0E
// J2534-2 SW_CAN
#define SW_CAN_HS						0x8000		/*-2*/
#define SW_CAN_NS						0x8001		/*-2*/
#define SET_POLL_RESPONSE				0x8002		/*-2*/
#define BECOME_MASTER					0x8003		/*-2*/

/*******************************/
/* Configuration Parameter IDs */
/*******************************/

#define DATA_RATE					0x01
// unused							0x02
#define LOOPBACK					0x03
#define NODE_ADDRESS				0x04
#define NETWORK_LINE				0x05
#define P1_MIN						0x06	// Don't use
#define P1_MAX						0x07
#define P2_MIN						0x08	// Don't use
#define P2_MAX						0x09	// Don't use
#define P3_MIN						0x0A
#define P3_MAX						0x0B	// Don't use
#define P4_MIN						0x0C
#define P4_MAX						0x0D	// Don't use
// See W0 = 0x19
#define W1							0x0E
#define W2							0x0F
#define W3							0x10
#define W4							0x11
#define W5							0x12
#define TIDLE						0x13
#define TINIL						0x14
#define TWUP						0x15
#define PARITY						0x16
#define BIT_SAMPLE_POINT			0x17
#define SYNC_JUMP_WIDTH				0x18
#define W0							0x19
#define T1_MAX						0x1A
#define T2_MAX						0x1B
// See T3_MAX						0x24
#define T4_MAX						0x1C
#define T5_MAX						0x1D
#define ISO15765_BS					0x1E
#define ISO15765_STMIN				0x1F
#define DATA_BITS					0x20
#define FIVE_BAUD_MOD				0x21
#define BS_TX						0x22
#define STMIN_TX					0x23
#define T3_MAX						0x24
#define ISO15765_WFT_MAX			0x25

// J2534-2
#define CAN_MIXED_FORMAT			0x8000	/*-2*/

// J1962_PINS: value = 0xPPSS 
// PP= primary pin, usually positive (eg CAN hi-signal)
// SS= secondary pin (usually negative) or maybe ISO L-line), can be omitted (0x00) if not needed
// result: STATUS_NOERROR if ok
//	ERR_PIN_INVALID if invalid or dangerous pin combination
//	ERR_NOT_SUPPORTED if not supported by HW
#define J1962_PINS					0x8001	/*-2*/		

#define SW_CAN_HS_DATA_RATE			0x8010	/*-2*/
#define SW_CAN_SPEEDCHANGE_ENABLE	0x8011	/*-2*/
#define SW_CAN_RES_SWITCH			0x8012	/*-2*/
#define ACTIVE_CHANNELS				0x8020	// Bitmask of channels being sampled
#define SAMPLE_RATE					0x8021	// Samples/second or Seconds/sample
#define SAMPLES_PER_READING			0x8022	// Samples to average into a single reading
#define READINGS_PER_MSG			0x8023	// Number of readings for each active channel per PASSTHRU_MSG structure
#define AVERAGING_METHOD			0x8024	// The way in which the samples will be averaged.
#define SAMPLE_RESOLUTION			0x8025	// The number of bits of resolution for each channel in the subsystem. Read Only.
#define INPUT_RANGE_LOW				0x8026	// Lower limit in millivolts of A/D input. Read Only.
#define INPUT_RANGE_HIGH			0x8027	// Upper limit in millivolts of A/D input. Read Only.

/*************/
/* Error IDs */
/*************/

// Function call successful
#define STATUS_NOERROR				0x00

// Device cannot support requested functionality mandated in this
// document. Device is not fully SAE J2534 compliant
#define ERR_NOT_SUPPORTED			0x01

// Invalid ChannelID value
#define ERR_INVALID_CHANNEL_ID		0x02

// Invalid ProtocolID value, unsupported ProtocolID, or there is a resource conflict (i.e. trying to connect to
// multiple protocols that are mutually exclusive such as J1850PWM and J1850VPW, or CAN and SCI A, etc.)
#define ERR_INVALID_PROTOCOL_ID		0x03

// NULL pointer supplied where a valid pointer is required
#define ERR_NULL_PARAMETER			0x04

// Invalid value for Ioctl parameter
#define ERR_INVALID_IOCTL_VALUE		0x05

// Invalid flag values
#define ERR_INVALID_FLAGS			0x06

// Undefined error, use PassThruGetLastError for text description
#define ERR_FAILED					0x07

// Device ID invalid
#define ERR_DEVICE_NOT_CONNECTED	0x08

// Timeout.
// PassThruReadMsg: No message available to read or could not read the specified number of
//   messages. The actual number of messages read is placed in <NumMsgs>
// PassThruWriteMsg: Device could not write the specified number of messages. The actual number of
//   messages sent on the vehicle network is placed in <NumMsgs>.
#define ERR_TIMEOUT					0x09

// Invalid message structure pointed to by pMsg (Reference Section 8 – Message Structure)
#define ERR_INVALID_MSG				0x0A

// Invalid TimeInterval value
#define ERR_INVALID_TIME_INTERVAL	0x0B

// Exceeded maximum number of message IDs or allocated space
#define ERR_EXCEEDED_LIMIT			0x0C

// Invalid MsgID value
#define ERR_INVALID_MSG_ID			0x0D

// Device is currently open
#define ERR_DEVICE_IN_USE			0x0E

// Invalid IoctlID value
#define ERR_INVALID_IOCTL_ID		0x0F

// Protocol message buffer empty, no messages available to read
#define ERR_BUFFER_EMPTY			0x10

// Protocol message buffer full. All the messages specified may not have been transmitted
#define ERR_BUFFER_FULL				0x11

// Indicates a buffer overflow occurred and messages were lost
#define ERR_BUFFER_OVERFLOW			0x12

// Invalid pin number, pin number already in use, or voltage already applied to a different pin
#define ERR_PIN_INVALID				0x13

// Channel number is currently connected
#define ERR_CHANNEL_IN_USE			0x14

// Protocol type in the message does not match the protocol associated with the Channel ID
#define ERR_MSG_PROTOCOL_ID			0x15

// Invalid Filter ID value
#define ERR_INVALID_FILTER_ID		0x16

// No flow control filter set or matched (for protocolID ISO15765 only)
#define ERR_NO_FLOW_CONTROL			0x17

// A CAN ID in pPatternMsg or pFlowControlMsg matches either ID in an existing FLOW_CONTROL_FILTER
#define ERR_NOT_UNIQUE				0x18

// The desired baud rate cannot be achieved within the tolerance specified in Section 6.5
#define ERR_INVALID_BAUDRATE		0x19

// Unable to communicate with device
#define ERR_INVALID_DEVICE_ID		0x1A

#define ERR_NULLPARAMETER			ERR_NULL_PARAMETER	/*v2*/


/*****************************/
/* Miscellaneous definitions */
/*****************************/
#define SHORT_TO_GROUND				0xFFFFFFFE
#define VOLTAGE_OFF					0xFFFFFFFF

#define NO_PARITY					0
#define ODD_PARITY					1
#define EVEN_PARITY					2

//SWCAN
#define DISBLE_SPDCHANGE			0	/*-2*/
#define ENABLE_SPDCHANGE			1	/*-2*/
#define DISCONNECT_RESISTOR			0	/*-2*/
#define CONNECT_RESISTOR			1	/*-2*/
#define AUTO_RESISTOR				2	/*-2*/

//Mixed Mode
#define CAN_MIXED_FORMAT_OFF			0	/*-2*/
#define CAN_MIXED_FORMAT_ON				1	/*-2*/
#define CAN_MIXED_FORMAT_ALL_FRAMES		2	/*-2*/


/*******************************/
/* PassThruConnect definitions */
/*******************************/

// 0 = Receive standard CAN ID (11 bit)
// 1 = Receive extended CAN ID (29 bit)
#define CAN_29BIT_ID					0x00000100

// 0 = The interface will generate and append the checksum as defined in ISO 9141-2 and ISO 14230-2 for
// transmitted messages, and verify the checksum for received messages.
// 1 = The interface will not generate and verify the checksum-the entire message will be treated as
// data by the interface
#define ISO9141_NO_CHECKSUM				0x00000200

// 0 = either standard or extended CAN ID types used – CAN ID type defined by bit 8
// 1 = both standard and extended CAN ID types used – if the CAN controller allows prioritizing either standard
// (11 bit) or extended (29 bit) CAN ID's then bit 8 will determine the higher priority ID type
#define CAN_ID_BOTH						0x00000800

// 0 = use L-line and K-line for initialization address
// 1 = use K-line only line for initialization address
#define ISO9141_K_LINE_ONLY				0x00001000

/************************/
/* RxStatus definitions */
/************************/

// 0 = received i.e. this message was transmitted on the bus by another node
// 1 = transmitted i.e. this is the echo of the message transmitted by the PassThru device
#define TX_MSG_TYPE						0x00000001

// 0 = Not a start of message indication
// 1 = First byte or frame received
#define START_OF_MESSAGE				0x00000002
#define ISO15765_FIRST_FRAME			0x00000002	/*v2 compat from v0202*/

#define ISO15765_EXT_ADDR				0x00000080	/*DT Accidentally refered to in spec*/

// 0 = No break received
// 1 = Break received
#define RX_BREAK						0x00000004

// 0 = No TxDone
// 1 = TxDone
#define TX_INDICATION					0x00000008	// Preferred name
#define TX_DONE							0x00000008

// 0 = No Error
// 1 = Padding Error
#define ISO15765_PADDING_ERROR			0x00000010

// 0 = no extended address,
// 1 = extended address is first byte after the CAN ID
#define ISO15765_ADDR_TYPE				0x00000080

//CAN_29BIT_ID							0x00000100  defined above

#define	SW_CAN_NS_RX					0x00040000	/*-2*/
#define	SW_CAN_HS_RX					0x00020000	/*-2*/
#define	SW_CAN_HV_RX					0x00010000	/*-2*/

/***********************/
/* TxFlags definitions */
/***********************/

// 0 = no padding
// 1 = pad all flow controlled messages to a full CAN frame using zeroes
#define ISO15765_FRAME_PAD				0x00000040

//ISO15765_ADDR_TYPE					0x00000080  defined above
//CAN_29BIT_ID							0x00000100  defined above

// 0 = Interface message timing as specified in ISO 14230
// 1 = After a response is received for a physical request, the wait time shall be reduced to P3_MIN
// Does not affect timing on responses to functional requests
#define WAIT_P3_MIN_ONLY				0x00000200

#define SW_CAN_HV_TX					0x00000400	/*-2*/

// 0 = Transmit using SCI Full duplex mode
// 1 = Transmit using SCI Half duplex mode
#define SCI_MODE						0x00400000

// 0 = no voltage after message transmit
// 1 = apply 20V after message transmit
#define SCI_TX_VOLTAGE					0x00800000

#define DT_PERIODIC_UPDATE				0x10000000	/*DT*/


/**********************/
/* Filter definitions */
/**********************/

// Allows matching messages into the receive queue. This filter type is only valid on non-ISO 15765 channels
#define PASS_FILTER						0x00000001

// Keeps matching messages out of the receive queue. This filter type is only valid on non-ISO 15765 channels
#define BLOCK_FILTER					0x00000002

// Allows matching messages into the receive queue and defines an outgoing flow control message to support
// the ISO 15765-2 flow control mechanism. This filter type is only valid on ISO 15765 channels.
#define FLOW_CONTROL_FILTER				0x00000003

#pragma pack(push,1)
typedef struct _PASSTHRU_MSG
{
	// Protocol type
	unsigned long ProtocolID;

	// Receive message status – See RxStatus in "Message Flags and Status Definition" section
	unsigned long RxStatus;

	// Transmit message flags
	unsigned long TxFlags;

	// Received message timestamp (microseconds): For the START_OF_FRAME
	// indication, the timestamp is for the start of the first bit of the message. For all other
	// indications and transmit and receive messages, the timestamp is the end of the last
	// bit of the message. For all other error indications, the timestamp is the time the error
	// is detected.
	unsigned long Timestamp;

	// Data size in bytes, including header bytes, ID bytes, message data bytes, and extra
	// data, if any.
	unsigned long DataSize;

	// Start position of extra data in received message (for example, IFR). The extra data
	// bytes follow the body bytes in the Data array. The index is zero-based. When no
	// extra data bytes are present in the message, ExtraDataIndex shall be set equal to
	// DataSize. Therefore, if DataSize equals ExtraDataIndex, there are no extra data
	// bytes. If ExtraDataIndex=0, then all bytes in the data array are extra bytes.
	unsigned long ExtraDataIndex;

	// Start position of extra data in received message (for example, IFR). The extra data
	// bytes follow the body bytes in the Data array. The index is zero-based. When no
	// extra data bytes are present in the message, ExtraDataIndex shall be set equal to
	// DataSize. Therefore, if DataSize equals ExtraDataIndex, there are no extra data
	// bytes. If ExtraDataIndex=0, then all bytes in the data array are extra bytes.
	unsigned char Data[4128];
} PASSTHRU_MSG;

typedef struct _SCONFIG
{	
	unsigned long Parameter;		// Name of parameter
	unsigned long Value;			// Value of the parameter
} SCONFIG;

typedef struct _SCONFIG_LIST
{
	unsigned long NumOfParams;		// Number of SCONFIG elements
	SCONFIG *ConfigPtr;				// Array of SCONFIG
} SCONFIG_LIST;

typedef struct _SBYTE_ARRAY
{
	unsigned long NumOfBytes;		// Number of bytes in the array
	unsigned char *BytePtr;			// Array of bytes
} SBYTE_ARRAY;
#pragma pack(pop)

#ifdef _WIN32
	#define J2534_API __stdcall
#else
	#define J2534_API
#endif

typedef long (J2534_API* PTOPEN)(void *pName, unsigned long *pDeviceID);
typedef long (J2534_API* PTCLOSE)(unsigned long DeviceID);
typedef long (J2534_API* PTCONNECT)(unsigned long DeviceID, unsigned long ProtocolID, unsigned long Flags, unsigned long BaudRate, unsigned long *pChannelID);
typedef long (J2534_API* PTDISCONNECT)(unsigned long ChannelID);
typedef long (J2534_API* PTREADMSGS)(unsigned long ChannelID, PASSTHRU_MSG *pMsg, unsigned long *pNumMsgs, unsigned long Timeout);
typedef long (J2534_API* PTWRITEMSGS)(unsigned long ChannelID, PASSTHRU_MSG *pMsg, unsigned long *pNumMsgs, unsigned long Timeout);
typedef long (J2534_API* PTSTARTPERIODICMSG)(unsigned long ChannelID, PASSTHRU_MSG *pMsg, unsigned long *pMsgID, unsigned long TimeInterval);
typedef long (J2534_API* PTSTOPPERIODICMSG)(unsigned long ChannelID, unsigned long MsgID);
typedef long (J2534_API* PTSTARTMSGFILTER)(unsigned long ChannelID, unsigned long FilterType, PASSTHRU_MSG *pMaskMsg, PASSTHRU_MSG *pPatternMsg, PASSTHRU_MSG *pFlowControlMsg, unsigned long *pFilterID);
typedef long (J2534_API* PTSTOPMSGFILTER)(unsigned long ChannelID, unsigned long FilterID);
typedef long (J2534_API* PTSETPROGRAMMINGVOLTAGE)(unsigned long DeviceID, unsigned long PinNumber, unsigned long Voltage);
typedef long (J2534_API* PTREADVERSION)(unsigned long DeviceID, char *pFirmwareVersion, char *pDllVersion, char *pApiVersion);
typedef long (J2534_API* PTGETLASTERROR)(char *pErrorDescription);
typedef long (J2534_API* PTIOCTL)(unsigned long ChannelID, unsigned long IoctlID, void *pInput, void *pOutput);
