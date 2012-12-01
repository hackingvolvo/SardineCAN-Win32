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

#include <stdafx.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include "j2534_v0404.h"
#include "shim_debug.h"
//#include "shim_frontend.h"
//#include "shim_output.h"
#include "helper.h"

// In case of some internal errors we'll return ERR_FAILED, set our own internal string,
// and return that until the app makes another PassThru function call
bool fUseLastInternalError = false;
TCHAR szLastInternalError[80] = {0};
void shim_setInternalError(LPCTSTR format, ...)
{
	va_list	args;

	// Generate the actual debug string
	va_start(args, format);
	_vsntprintf_s(szLastInternalError, sizeof(szLastInternalError)/sizeof(TCHAR), _TRUNCATE, format, args);
	va_end(args);

	fUseLastInternalError = true;
}
LPCTSTR shim_getInternalError()
{
	return szLastInternalError;
}
void shim_clearInternalError()
{
	_tcscpy_s(szLastInternalError, sizeof(szLastInternalError)/sizeof(szLastInternalError[0]), _T("No internal error"));
	fUseLastInternalError = false;
}
bool shim_hadInternalError()
{
	return fUseLastInternalError;
}

/*
void dbug_printretval(unsigned long retval)
{
	if (retval == STATUS_NOERROR ||
		retval == ERR_TIMEOUT ||
		retval == ERR_BUFFER_EMPTY)
	{
		dtDebug(_T("  %.3fs %s\n"), GetTimeSinceInit(), dbug_return(retval).c_str());
	}
	else
	{
		char szErrorDescription[80];
		shim_PassThruGetLastError(szErrorDescription);
		CStringW cstrErrorDescription(szErrorDescription);
		dtDebug(_T("  %.3fs %s '%s'\n"), GetTimeSinceInit(), dbug_return(retval).c_str(), cstrErrorDescription);
	}
}
*/

static LPCTSTR dbug_retval2str(unsigned long RetVal)
{
	switch (RetVal)
	{
	case STATUS_NOERROR:			return _T("STATUS_NOERROR");				// Assigned in J2534-1
	case ERR_NOT_SUPPORTED:			return _T("ERR_NOT_SUPPORTED");
	case ERR_INVALID_CHANNEL_ID:	return _T("ERR_INVALID_CHANNEL_ID");
	case ERR_INVALID_PROTOCOL_ID:	return _T("ERR_INVALID_PROTOCOL_ID");
	case ERR_NULL_PARAMETER:		return _T("ERR_NULL_PARAMETER");
	case ERR_INVALID_IOCTL_VALUE:	return _T("ERR_INVALID_IOCTL_VALUE");
	case ERR_INVALID_FLAGS:			return _T("ERR_INVALID_FLAGS");
	case ERR_FAILED:				return _T("ERR_FAILED");
	case ERR_DEVICE_NOT_CONNECTED:	return _T("ERR_DEVICE_NOT_CONNECTED");
	case ERR_TIMEOUT:				return _T("ERR_TIMEOUT");
	case ERR_INVALID_MSG:			return _T("ERR_INVALID_MSG");
	case ERR_INVALID_TIME_INTERVAL:	return _T("ERR_INVALID_TIME_INTERVAL");
	case ERR_EXCEEDED_LIMIT:		return _T("ERR_EXCEEDED_LIMIT");
	case ERR_INVALID_MSG_ID:		return _T("ERR_INVALID_MSG_ID");
	case ERR_DEVICE_IN_USE:			return _T("ERR_DEVICE_IN_USE");
	case ERR_INVALID_IOCTL_ID:		return _T("ERR_INVALID_IOCTL_ID");
	case ERR_BUFFER_EMPTY:			return _T("ERR_BUFFER_EMPTY");
	case ERR_BUFFER_FULL:			return _T("ERR_BUFFER_FULL");
	case ERR_BUFFER_OVERFLOW:		return _T("ERR_BUFFER_OVERFLOW");
	case ERR_PIN_INVALID:			return _T("ERR_PIN_INVALID");
	case ERR_CHANNEL_IN_USE:		return _T("ERR_CHANNEL_IN_USE");
	case ERR_MSG_PROTOCOL_ID:		return _T("ERR_MSG_PROTOCOL_ID");
	case ERR_INVALID_FILTER_ID:		return _T("ERR_INVALID_FILTER_ID");
	case ERR_NO_FLOW_CONTROL:		return _T("ERR_NO_FLOW_CONTROL");
	case ERR_NOT_UNIQUE:			return _T("ERR_NOT_UNIQUE");
	case ERR_INVALID_BAUDRATE:		return _T("ERR_INVALID_BAUDRATE");
	case ERR_INVALID_DEVICE_ID:		return _T("ERR_INVALID_DEVICE_ID");
	default: break;
	}

	if (RetVal >= 0x00010000 && RetVal <= 0xFFFFFFFF)
	{
		return _T("?J2534-2?");
	}
	else if (RetVal >= 0x0000001B && RetVal <= 0x0000FFFF)
	{
		return _T("?J2534-1?");
	}

	return _T("?retval?");
}

tstring dbug_return(unsigned long RetVal)
{
	std::basic_ostringstream<wchar_t> ssRetVal;

	ssRetVal << RetVal << _T(":") << dbug_retval2str(RetVal);

	return ssRetVal.str();
}

LPCTSTR dbug_filter2str(unsigned long FilterType)
{
	switch (FilterType)
	{
	case PASS_FILTER:	return _T("PASS_FILTER");
	case BLOCK_FILTER:	return _T("BLOCK_FILTER");
	case FLOW_CONTROL_FILTER:	return _T("FLOW_CONTROL_FILTER");
	default: break;
	}

	if (FilterType >= 0x00010000 && FilterType <= 0xFFFFFFFF)
	{
		return _T("?vendor?");
	}
	else if (FilterType >= 0x00008000 && FilterType <= 0x0000FFFF)
	{
		return _T("?J2534-2?");
	}
	else if (FilterType >= 0x00000004 && FilterType <= 0x00007FFF)
	{
		return _T("?SAE?");
	}
	return _T("?filter?");
}

tstring dbug_filter(unsigned long FilterType)
{
	std::basic_ostringstream<wchar_t> ssFilterType;

	ssFilterType << FilterType << _T(":") << dbug_filter2str(FilterType);

	return ssFilterType.str();
}

 LPCTSTR dbug_ioctl2str(unsigned long IoctlID)
{
	switch (IoctlID)
	{

		case GET_CONFIG:			return _T("GET_CONFIG");			// Assigned in J2534-1
		case SET_CONFIG:			return _T("SET_CONFIG");
		case READ_VBATT:			return _T("READ_VBATT");
		case FIVE_BAUD_INIT:		return _T("FIVE_BAUD_INIT");
		case FAST_INIT:				return _T("FAST_INIT");
		case CLEAR_TX_BUFFER:		return _T("CLEAR_TX_BUFFER");
		case CLEAR_RX_BUFFER:		return _T("CLEAR_RX_BUFFER");
		case CLEAR_PERIODIC_MSGS:	return _T("CLEAR_PERIODIC_MSGS");
		case CLEAR_MSG_FILTERS:		return _T("CLEAR_MSG_FILTERS");
		case CLEAR_FUNCT_MSG_LOOKUP_TABLE:			return _T("CLEAR_FUNCT_MSG_LOOKUP_TABLE");
		case ADD_TO_FUNCT_MSG_LOOKUP_TABLE:			return _T("ADD_TO_FUNCT_MSG_LOOKUP_TABLE");
		case DELETE_FROM_FUNCT_MSG_LOOKUP_TABLE:	return _T("DELETE_FROM_FUNCT_MSG_LOOKUP_TABLE");
		case READ_PROG_VOLTAGE:		return _T("READ_PROG_VOLTAGE");
		case SW_CAN_HS:				return _T("SW_CAN_HS");					// Assigned in J2534-2
		case SW_CAN_NS:				return _T("SW_CAN_NS");
		case SET_POLL_RESPONSE:		return _T("SET_POLL_RESPONSE");
		case BECOME_MASTER:			return _T("BECOME_MASTER");

		//case DT_IOCTL_VVSTATS:		return _T("DT_IOCTL_VVSTATS");
		//case READ_ANALOG_CH1:		return _T("READ_ANALOG_CH1");
		//case READ_ANALOG_CH2:		return _T("READ_ANALOG_CH2");
		//case READ_ANALOG_CH3:		return _T("READ_ANALOG_CH3");
		//case READ_ANALOG_CH4:		return _T("READ_ANALOG_CH4");
		//case READ_ANALOG_CH5:		return _T("READ_ANALOG_CH5");
		//case READ_ANALOG_CH6:		return _T("READ_ANALOG_CH6");
		//case READ_CH1_VOLTAGE:		return _T("READ_CH1_VOLTAGE");
		//case READ_CH2_VOLTAGE:		return _T("READ_CH2_VOLTAGE");
		//case READ_CH3_VOLTAGE:		return _T("READ_CH3_VOLTAGE");
		//case READ_CH4_VOLTAGE:		return _T("READ_CH4_VOLTAGE");
		//case READ_CH5_VOLTAGE:		return _T("READ_CH5_VOLTAGE");
		//case READ_CH6_VOLTAGE:		return _T("READ_CH6_VOLTAGE");
		//case READ_TIMESTAMP:		return _T("READ_TIMESTAMP");
		//case DT_READ_DIO: return "DT_READ_DIO";
		//case DT_WRITE_DIO: return "DT_WRITE_DIO";
		default: break;
	}

	if (IoctlID >= 0x00010000 && IoctlID <= 0xFFFFFFFF)
	{
		return _T("?vendor?");
	}
	else if (IoctlID >= 0x00008000 && IoctlID <= 0x0000FFFF)
	{
		return _T("?J2534-2?");
	}
	else if (IoctlID >= 0x0000000F && IoctlID <= 0x00007FFF)
	{
		return _T("?SAE?");
	}
	return _T("?ioctl?");
}

tstring dbug_ioctl(unsigned long IoctlID)
{
	std::basic_ostringstream<wchar_t> ssIoctlID;

	ssIoctlID << IoctlID << _T(":") << dbug_ioctl2str(IoctlID);

	return ssIoctlID.str();
}

 LPCTSTR dbug_param2str(unsigned long ParamID)
{
	switch (ParamID)
	{
		case DATA_RATE:			return _T("DATA_RATE");				// Assigned in J2534-1
		case LOOPBACK:			return _T("LOOPBACK");
		case NODE_ADDRESS:		return _T("NODE_ADDRESS");
		case NETWORK_LINE:		return _T("NETWORK_LINE");
		case P1_MIN:			return _T("P1_MIN");
		case P1_MAX:			return _T("P1_MAX");
		case P2_MIN:			return _T("P2_MIN");
		case P2_MAX:			return _T("P2_MAX");
		case P3_MIN:			return _T("P3_MIN");
		case P3_MAX:			return _T("P3_MAX");
		case P4_MIN:			return _T("P4_MIN");
		case P4_MAX:			return _T("P4_MAX");
		case W0:				return _T("W0");
		case W1:				return _T("W1");
		case W2:				return _T("W2");
		case W3:				return _T("W3");
		case W4:				return _T("W4");
		case W5:				return _T("W5");
		case TIDLE:				return _T("TIDLE");
		case TINIL:				return _T("TINIL");
		case TWUP:				return _T("TWUP");
		case PARITY:			return _T("PARITY");
		case BIT_SAMPLE_POINT:	return _T("BIT_SAMPLE_POINT");
		case SYNC_JUMP_WIDTH:	return _T("SYNC_JUMP_WIDTH");
		case T1_MAX:			return _T("T1_MAX");
		case T2_MAX:			return _T("T2_MAX");
		case T3_MAX:			return _T("T3_MAX");
		case T4_MAX:			return _T("T4_MAX");
		case T5_MAX:			return _T("T5_MAX");
		case ISO15765_BS:		return _T("ISO15765_BS");
		case ISO15765_STMIN:	return _T("ISO15765_STMIN");
		case BS_TX:				return _T("BS_TX");
		case STMIN_TX:			return _T("STMIN_TX");
		case DATA_BITS:			return _T("DATA_BITS");
		case FIVE_BAUD_MOD:		return _T("FIVE_BAUD_MOD");
		case ISO15765_WFT_MAX:	return _T("ISO15765_WFT_MAX");
		case CAN_MIXED_FORMAT:	return _T("CAN_MIXED_FORMAT");		// Assigned in J2534-2
		case J1962_PINS:		return _T("J1962_PINS");
		case SW_CAN_HS_DATA_RATE:	return _T("SW_CAN_HS_DATA_RATE");
		case SW_CAN_SPEEDCHANGE_ENABLE:	return _T("SW_CAN_SPEEDCHANGE_ENABLE");
		case SW_CAN_RES_SWITCH: return _T("SW_CAN_RES_SWITCH");
		case ACTIVE_CHANNELS:	return _T("ACTIVE_CHANNELS");
		case SAMPLE_RATE:		return _T("SAMPLE_RATE");
		case SAMPLES_PER_READING:	return _T("SAMPLES_PER_READING");
		case READINGS_PER_MSG:	return _T("READINGS_PER_MSG");
		case AVERAGING_METHOD:	return _T("AVERAGING_METHOD");
		case SAMPLE_RESOLUTION:	return _T("SAMPLE_RESOLUTION");
		case INPUT_RANGE_LOW:	return _T("INPUT_RANGE_LOW");
		case INPUT_RANGE_HIGH:	return _T("INPUT_RANGE_HIGH");
		//case ISO15765_SIMULTANEOUS:	return _T("ISO15765_SIMULTANEOUS");
		//case DT_PARAM_FORD: return "DT_PARAM_FORD";
		//case DT_PARAM_NO_CKSM: return "DT_PARAM_NO_CKSM";
		//case DT_PARAM_ISOLB: return "DT_PARAM_ISOLB";
		//case DT_SNIFF_MODE: return "DT_SNIFF_MODE";
		//case DT_PARAM_29BITBOTH: return "DT_PARAM_29BITBOTH";
		//case DT_PARAM_DEFAULT_FILTER: return "DT_PARAM_DEFAULT_FILTER";
		//case DT_ISO15765_PAD_BYTE: return "DT_ISO15765_PAD_BYTE";
		//case ADC_READINGS_PER_SECOND:	return _T("ADC_READINGS_PER_SECOND");
		//case ADC_READINGS_PER_SAMPLE:	return _T("ADC_READINGS_PER_SAMPLE");
		//case DT_FILTER_FREQ: return "DT_FILTER_FREQ";
		default: break;
	}

	if (ParamID >= 0x00010000 && ParamID <= 0xFFFFFFFF)
	{
		return _T("?vendor?");
	}
	else if (ParamID >= 0x00008000 && ParamID <= 0x0000FFFF)
	{
		return _T("?J2534-2?");
	}
	else if (ParamID >= 0x00000026 && ParamID <= 0x00007FFF)
	{
		return _T("?SAE?");
	}
	return _T("?param?");
}

tstring dbug_param(unsigned long ParamID)
{
	std::basic_ostringstream<wchar_t> ssParamID;

	//ssParamID << _T("0x") << std::hex << std::setw(2) << std::setfill(_T('0')) << ParamID << _T(":") << dbug_param2str(ParamID);
	ssParamID << ParamID << _T(":") << dbug_param2str(ParamID);

	return ssParamID.str();
}

static LPCTSTR dbug_prot2str(unsigned long ProtocolID)
{
	switch (ProtocolID)
	{
		case J1850VPW:		return _T("J1850VPW");					// Assigned in J2534-1
		case J1850PWM:		return _T("J1850PWM");
		case ISO9141:		return _T("ISO9141");
		case ISO14230:		return _T("ISO14230");
		case CAN:			return _T("CAN");
		case ISO15765:		return _T("ISO15765");
		case SCI_A_ENGINE:	return _T("SCI_A_ENGINE");
		case SCI_A_TRANS:	return _T("SCI_A_TRANS");
		case SCI_B_ENGINE:	return _T("SCI_B_ENGINE");
		case SCI_B_TRANS:	return _T("SCI_B_TRANS");
		case J1850VPW_PS:	return _T("J1850VPW_PS");				// Assigned in J2534-2
		case J1850PWM_PS:	return _T("J1850PWM_PS");
		case ISO9141_PS:	return _T("ISO9141_PS");
		case ISO14230_PS:	return _T("ISO14230_PS");
		case J2610_PS:		return _T("J2610_PS");
		case SW_ISO15765_PS: return _T("SW_ISO15765_PS");
		case SW_CAN_PS:		return _T("SW_CAN_PS");
		case GM_UART_PS:	return _T("GM_UART_PS");
		//case CAN_XON_XOFF_PS:	return _T("CAN_XON_XOFF_PS");
		//case LIN_PS:		return _T("LIN_PS");
		//case J1708_PS:		return _T("J1708_PS");
		default: break;
	}

	if (ProtocolID >= 0x00010000 && ProtocolID <= 0xFFFFFFFF)
	{
		return _T("?vendor?");
	}
	else if (ProtocolID >= 0x00008000 && ProtocolID <= 0x0000FFFF)
	{
		return _T("?J2534-2?");
	}
	else if (ProtocolID >= 0x0000000B && ProtocolID <= 0x00007FFF)
	{
		return _T("?SAE?");
	}
	return _T("?protocol?");
}

tstring dbug_prot(unsigned long ProtocolID)
{
	std::basic_ostringstream<wchar_t> ssProtocolID;

	ssProtocolID << ProtocolID << _T(":") << dbug_prot2str(ProtocolID);

	return ssProtocolID.str();
}

static LPCTSTR dbug_cflag2str(unsigned long ConnectFlag)
{
	switch (ConnectFlag)
	{
	case CAN_29BIT_ID:		return _T("CAN_29BIT_ID");								// Assigned in J2534-1
	case ISO9141_NO_CHECKSUM:	return _T("ISO9141_NO_CHECKSUM");
	case CAN_ID_BOTH:		return _T("CAN_ID_BOTH");
	case ISO9141_K_LINE_ONLY:	return _T("ISO9141_K_LINE_ONLY");
	default: break;
	}

	if (ConnectFlag >= 0x01000000 && ConnectFlag <= 0x80000000)
	{
		return _T("?vendor?");
	}
	else if (ConnectFlag >= 0x00010000 && ConnectFlag <= 0x00800000)
	{
		return _T("?J2534-2?");
	}
	else if (ConnectFlag >= 0x00002000 && ConnectFlag <= 0x00008000)
	{
		return _T("?SAE?");
	}
	return _T("?cflag?");
}

void dbug_printcflag(unsigned long ConnectFlags)
{
	std::basic_ostringstream<wchar_t> ssConnectFlags;

	if (ConnectFlags == 0)
		return;

	ssConnectFlags << _T("  Flags:");
	for (int i=0; i < 32; i++)
	{
		unsigned long mask = 1 << i;

		if ((mask & ConnectFlags) == 0)
		{
			continue;
		}
		else
		{
			ssConnectFlags << _T(" ") << i << _T(":") << dbug_cflag2str(mask & ConnectFlags);
		}
	}
	ssConnectFlags << std::endl;

	dtDebug(ssConnectFlags.str().c_str());
}

static LPCTSTR dbug_rxstatus2str(unsigned long RxStatus)
{
	switch (RxStatus)
	{
	case TX_MSG_TYPE:		return _T("TX_MSG_TYPE");						// Assigned in J2534-1
	case START_OF_MESSAGE:	return _T("START_OF_MESSAGE");
	case RX_BREAK:			return _T("RX_BREAK");
	case TX_INDICATION:		return _T("TX_INDICATION");
	case ISO15765_PADDING_ERROR:	return _T("ISO15765_PADDING_ERROR");
	case ISO15765_ADDR_TYPE:		return _T("ISO15765_ADDR_TYPE");
	case CAN_29BIT_ID:		return _T("CAN_29BIT_ID");
	//case SWCAN_NS_RX: return "SWCAN_NS_RX";								// Assigned in J2534-2
	//case SWCAN_HS_RX: return "SWCAN_HS_RX";
	//case SWCAN_HV_RX: return "SWCAN_HV_RX";
	default: break;
	}

	if (RxStatus >= 0x01000000 && RxStatus <= 0x80000000)					// Bits 31-24
	{
		return _T("?vendor?");
	}
	else if (RxStatus >= 0x00010000 && RxStatus <= 0x00800000)				// Bits 23-16
	{
		return _T("?J2534-2?");
	}
	else if (RxStatus >= 0x00000100 && RxStatus <= 0x00008000)				// Bits 15-9
	{
		return _T("?SAE?");
	}
	else if (RxStatus >= 0x00000020 && RxStatus <= 0x00000040)				// Bits 6-5
	{
		return _T("?SAE?");
	}
	return _T("?rxstatus?");
}

void dbug_printrxstatus(unsigned long RxStatus)
{
	std::basic_ostringstream<wchar_t> ssRxStatus;

	if (RxStatus == 0)
		return;

	ssRxStatus << _T("  RxStatus:");
	for (int i=0; i < 32; i++)
	{
		unsigned long mask = 1 << i;

		if ((mask & RxStatus) == 0)
		{
			continue;
		}
		else
		{
			ssRxStatus << _T(" ") << i << _T(":") << dbug_rxstatus2str(mask & RxStatus);
		}
	}
	ssRxStatus << std::endl;

	dtDebug(ssRxStatus.str().c_str());
}

static LPCTSTR dbug_txflag2str(unsigned long TxFlags)
{
	switch (TxFlags)
	{
	case ISO15765_FRAME_PAD:	return _T("ISO15765_FRAME_PAD");			// Assigned in J2534-1
	case ISO15765_ADDR_TYPE:	return _T("ISO15765_ADDR_TYPE");
	case CAN_29BIT_ID:			return _T("CAN_29BIT_ID");
	case WAIT_P3_MIN_ONLY:		return _T("WAIT_P3_MIN_ONLY");
	case SCI_MODE:				return _T("SCI_MODE");
	case SCI_TX_VOLTAGE:		return _T("SCI_TX_VOLTAGE");
	//case SWCAN_HV_TX: return "SWCAN_HV_TX";								// Assigned in J2534-2
	default: break;
	}

	if (TxFlags >= 0x01000000 && TxFlags <= 0x80000000)						// Bits 31-24
	{
		return _T("?vendor?");
	}
	else if (TxFlags >= 0x00010000 && TxFlags <= 0x00200000)				// Bits 21-16
	{
		return _T("?J2534-2?");
	}
	else if (TxFlags >= 0x00000400 && TxFlags <= 0x00008000)				// Bits 15-10
	{
		return _T("?SAE?");
	}
	else if (TxFlags >= 0x00000001 && TxFlags <= 0x00000020)				// Bits 5-0
	{
		return _T("?SAE?");
	}
	return _T("?txflag?");
}

void dbug_printtxflags(unsigned long TxFlags)
{
	std::basic_ostringstream<wchar_t> ssTxFlags;

	if (TxFlags == 0)
		return;

	ssTxFlags << _T("  TxFlags:");
	for (int i=0; i < 32; i++)
	{
		unsigned long mask = 1 << i;

		if ((mask & TxFlags) == 0)
		{
			continue;
		}
		else
		{
			ssTxFlags << _T(" ") << i << _T(":") << dbug_txflag2str(mask & TxFlags);
		}
	}
	ssTxFlags << std::endl;

	dtDebug(ssTxFlags.str().c_str());
}

void dbug_printsbyte(SBYTE_ARRAY *inAry, LPCTSTR s)
{
	if (inAry == NULL)
	{
		dtDebug(_T("  %s is NULL\n"), s);
		return;
	}

	dtDebug(_T("  %s: %lu bytes at 0x%08X\n"), s, inAry->NumOfBytes, inAry->BytePtr);

	if (inAry->BytePtr == NULL)
	{
		dtDebug(_T("  %s->BytePtr is NULL\n"), inAry);
		return;
	}

	if (inAry->NumOfBytes > 0)
	{
		std::basic_ostringstream<wchar_t> ssData;

		ssData << std::hex << std::setfill(_T('0')) << _T("  \\__");
		for (unsigned long i=0; i < inAry->NumOfBytes; i++)
		{
			ssData << _T(" ") << std::setw(2) << inAry->BytePtr[i];
		}
		ssData << std::endl;

		dtDebug(ssData.str().c_str());
	}
}

void dbug_printsconfig(SCONFIG_LIST *pList)
{
	if (pList == NULL)
	{
		dtDebug(_T("  pList is NULL\n"));
		return;
	}

	dtDebug(_T("  %ld parameter(s) at 0x%08X:\n"), pList->NumOfParams, pList->ConfigPtr);
	if (pList->ConfigPtr == NULL)
	{
		dtDebug(_T("  pList->ConfigPtr is NULL\n"));
		return;
	}

	for (unsigned long i=0; i < pList->NumOfParams; i++)
	{
		dtDebug(_T("    %s = %ld\n"), dbug_param(pList->ConfigPtr[i].Parameter).c_str(), pList->ConfigPtr[i].Value);
	}
}

void dbug_printmsg(PASSTHRU_MSG mm[], LPCTSTR s, unsigned long * numMsgs, bool isWrite)
{
	if (mm == NULL)
		dtDebug(_T("  %s is NULL\n"), s);
	if (numMsgs == NULL)
		dtDebug(_T("  numMsgs is NULL\n"), s);

	if (mm == NULL || numMsgs == NULL)
		return;

	dbug_printmsg(mm, s, *numMsgs, isWrite);
}

void dbug_printmsg(PASSTHRU_MSG mm[], LPCTSTR s, unsigned long numMsgs, bool isWrite)
{
	if (mm == NULL)
	{
		dtDebug(_T("  %s is NULL\n"), s);
		return;
	}

	for (unsigned long i=0; i < numMsgs; i++)
	{
		if (isWrite == true)
		{
			dtDebug(_T("  %s[%2ld] %s. %lu bytes. TxF=0x%08lx\n"),
				s,
				i,
				//numMsgs,
				dbug_prot(mm[i].ProtocolID).c_str(),
				mm[i].DataSize,
				mm[i].TxFlags);
		}
		else
		{
			dtDebug(_T("  %s[%2ld] %fs. %s. Actual data %lu of %lu bytes. RxS=0x%08lx\n"),
				s,
				i,
				//numMsgs,
				mm[i].Timestamp / (float) 1000000,
				dbug_prot(mm[i].ProtocolID).c_str(),
				mm[i].ExtraDataIndex,
				mm[i].DataSize,
				mm[i].RxStatus);
		}

		// Display TxFlags if this is an outgoing message
		if (mm[i].TxFlags != 0 && isWrite == true)
		{
			dbug_printtxflags(mm[i].TxFlags);
		}

		// Display RxStatus if this is an incoming message
		if (mm[i].RxStatus != 0 && isWrite == false)
		{
			dbug_printrxstatus(mm[i].RxStatus);
		}

		// Display Data[] except for frames containing neither data nor extradata
		if (mm[i].DataSize > 0)
		{
			std::basic_ostringstream<wchar_t> ssData;
			unsigned long n = mm[i].ExtraDataIndex;

			ssData << std::hex << std::setfill(_T('0')) << _T("  \\__");
			for (unsigned long x = 0; x < mm[i].DataSize && x < sizeof(mm[i].Data); x++)
			{
				if (x < mm[i].ExtraDataIndex || isWrite == true)
				{
					ssData << _T(" ") << std::setw(2) << mm[i].Data[x];
				}
				else
				{
					ssData << _T(" [") << std::setw(2) << mm[i].Data[x] << _T("]");
				}
			}
			ssData << std::endl;

			dtDebug(ssData.str().c_str());
		}
	}
}