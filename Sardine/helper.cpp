#include "stdafx.h"
#include "helper.h"
#include "sardine_defs.h"
#include "shim_debug.h"
#include <stdio.h>

namespace debug {
	// FIXME: load default values from Windows registry, and add DLL functions to change these settings
	unsigned long debug_fields = ERR | INIT | MAINFUNC | HELPERFUNC | ARDUINO_MSG | PROTOCOL | PROTOCOL_VERBOSE | PROTOCOL_MSG | PROTOCOL_MSG_VERBOSE | ARDUINO_MSG_VERBOSE;
}

int get_stack_trace_depth()
{
	unsigned long prev;
	unsigned long addr;
	int i=0;
	__asm { mov prev, ebp }
	while(addr!=0) { 
		addr = ((unsigned long *)prev)[1]; 
		//	  printf("0x%08x\n", addr); 
		prev = ((unsigned long *)prev)[0]; 
		i++;
	}
	return i-1;
}

void PrintError( int error )
{
	LPSTR errorText = NULL;
	FormatMessageA(
	   // use system message tables to retrieve error text
	   FORMAT_MESSAGE_FROM_SYSTEM
	   // allocate buffer on local heap for error text
	   |FORMAT_MESSAGE_ALLOCATE_BUFFER
	   // Important! will fail otherwise, since we're not 
	   // (and CANNOT) pass insertion parameters
	   |FORMAT_MESSAGE_IGNORE_INSERTS,  
	   NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
	   error,
	   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	   (LPSTR)&errorText,  // output 
	   0, // minimum size for output buffer
	   NULL);   // arguments - see note
	if (errorText != NULL)
	{
		LOG(ERR,"Error: [%d] %s", error, errorText);
	}
}

char* ConvertLPWSTRToLPSTR (LPWSTR lpwszStrIn)
{
  LPSTR pszOut = NULL;
  if (lpwszStrIn != NULL)
  {
	int nInputStrLen = wcslen (lpwszStrIn);

	// Double NULL Termination
	int nOutputStrLen = WideCharToMultiByte (CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
	pszOut = new char [nOutputStrLen];

	if (pszOut)
	{
	  memset (pszOut, 0x00, nOutputStrLen);
	  WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
	}
  }
  return pszOut;
}


	void LogMessage( PASSTHRU_MSG * pMsg, LogMessageType msgType, unsigned long channelId, char * comment )
{
#ifdef ENABLE_MSG_LOGGING
	 char szMessageBuffer[1024] = {0};	
	 SYSTEMTIME systemTime;				
	 GetSystemTime( &systemTime );		
	 sprintf_s(szMessageBuffer, 1024, "[%02d:%02d:%02d.%03d] ",systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds);
	 std::ofstream handle;				
	 handle.open("c:\\sardine\\sardine-msg.log",std::ios_base::app); 
	 handle << szMessageBuffer;

	 switch (msgType)
	 {
	 case LogMessageType::RECEIVED:
		 handle << ">>>>";
		 break;
	 case LogMessageType::SENT:
		 handle << "<<<< ";
		 break;
	 case LogMessageType::ISO15765_RECV:
		 handle << "ISO>";
		 break;
	 case LogMessageType::ISO15765_SENT:
		 handle << "<ISO";
		 break;
	 case LogMessageType::LOOP_BACK:
		 handle << "LOOP";
		 break;
	 default:
		 handle << "UDEF";
	 }

	 sprintf_s(szMessageBuffer,1024," Ch#%02d",channelId);
	 handle << szMessageBuffer;

	 unsigned long pid = pMsg->ProtocolID;
	 if (pid >= 0x8000)
		 {
		 handle << " PS ";	// pin switching
		 pid -= 0x8000-1;  // PS ids start from 0x8000, normal (without pin switching) from 0x0001
		 }
	 else
		 handle << "    ";
	 switch (pid)
	 {
		 case J1850VPW:
			 handle << "J1850VPW ";
			break;
		 case J1850PWM:
			 handle << "J1850PWM ";
			break;
		 case ISO9141:
			 handle << "ISO9141  ";
			break;
		 case ISO14230:
			 handle << "ISO14230 ";
			break;
		 case CAN:
			 handle << "CAN      ";
			break;
		 case ISO15765:
			 handle << "ISO15765 ";
			break;
		 default:
			 handle << "UNKNOWN  ";
	 }
	 if ( (msgType==RECEIVED) || (msgType==ISO15765_RECV) || (msgType==LOOP_BACK) )
	 {
		 if ( (pid==CAN) || (pid==ISO15765) )
		 {
			 if (pMsg->RxStatus & CAN_29BIT_ID)
				 handle << "29b ";
			 else
				 handle << "11b ";
			 if (pMsg->RxStatus & ISO15765_ADDR_TYPE)
				 handle << "EXT ADDR ";
			 else
				 handle << "STD ADDR ";
		 }

	 } else
	if ( (msgType==SENT) || (msgType==ISO15765_SENT) )
	{
		 if ( (pid==CAN) || (pid==ISO15765) )
		 {
			 if (pMsg->TxFlags & CAN_29BIT_ID)
				 handle << "29b ";
			 else
				 handle << "11b ";
			 if (pMsg->TxFlags & ISO15765_ADDR_TYPE)
				 handle << "EXT ADDR ";
			 else
				 handle << "STD ADDR ";
		 }
	}

	 sprintf_s(szMessageBuffer, 1024, "DS:%02d EDI:%02d :: ",pMsg->DataSize,pMsg->ExtraDataIndex);
	 handle << szMessageBuffer;	
	 sprintf_s(szMessageBuffer, 1024, "%02x %02x %02x %02x | ",
		 pMsg->Data[0],
		 pMsg->Data[1],
		 pMsg->Data[2],
		 pMsg->Data[3]);
	 handle << szMessageBuffer;	
	 for (unsigned int i=4;i<pMsg->DataSize;i++)
	 {
		 sprintf_s(szMessageBuffer, 1024, "%02x ",pMsg->Data[i]);
		 handle << szMessageBuffer;	
	 }
		 
	 if (comment)
		 handle << comment;

	 handle << "\n" << std::flush;					
#endif
}




/*
void dtDebug(LPCTSTR message, ...)
{
 TCHAR szMessageBuffer[1024] = {0}; 
 char szMessageBufferAsc[1024] = {0}; 
 SYSTEMTIME systemTime; 
 GetSystemTime( &systemTime ); 
 sprintf_s(szMessageBufferAsc, 1024, "[%2d:%2d:%2d.%3d] ",systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds); 
 std::ofstream handle;  
 handle.open("c:\\temp\\sardinetest.log",std::ios_base::app);  
 handle << szMessageBufferAsc; 
 std::cout << szMessageBufferAsc; 
 swprintf_s(szMessageBuffer, 1024, message, __VA_ARGS__); 
 char * ascbuffer = ConvertLPWSTRToLPSTR( szMessageBuffer ) ; 
 handle << ascbuffer; 
 std::cout << ascbuffer; 
 delete ascbuffer; 
 handle << "\n"; 
 std::cout << "\n"; 

	/*	va_list	args;
	va_start(args, format);

	if (fLogToFile)
	{
		// Send this directly to the file
		_vftprintf_s(fp, format, args);
	}
	else
	{
		// Send this to the circular memory-buffer
		TCHAR temp[100];
		_vsntprintf_s(temp, sizeof(temp)/sizeof(temp[0]), _TRUNCATE, format, args);
		logFifo.Put(temp);
	}

	va_end(args);
	*/
/*
  va_list args;
   va_start(args, format);
   int r = vprintf(format, args);
   va_end(args);
   return r;

	LOGW(HELPERFUNC,format,__VA_ARGS__);
	*/




/*
static LPCTSTR dbug_ioctl2str(unsigned long IoctlID)
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
*/


void Print_IOCtl_Cmd( unsigned long IoctlID )
{
	LOGW(HELPERFUNC,_T("IOCTL Command: %d [%s]"),IoctlID, dbug_ioctl2str(IoctlID));
}

/*
static LPCTSTR dbug_param2str(unsigned long ParamID)
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
*/

void Print_SByte_Array(SBYTE_ARRAY * pArray)
	{
	if (pArray == NULL)
	{
		LOG(ERR,"Print_SByte_Array: pArray == NULL");
		return;
	}
	LOG_BYTES(HELPERFUNC, pArray->NumOfBytes, pArray->BytePtr);
//	LOG(HELPERFUNC,"Print_SByte_Array: byte count: %d",pArray->NumOfBytes);
	}

void Print_SConfig_List(SCONFIG_LIST *pList)
{
	if (pList == NULL)
	{
		LOG(ERR,"Print_SConfig_List: pList == NULL");
		return;
	}

	LOG(HELPERFUNC,"Print_SConfig_List: param count: %d",pList->NumOfParams);
	if (pList->ConfigPtr == NULL)
	{
		LOG(ERR,"Print_SConfig_List: ConfigPtr == NULL");
		return;
	}

	for (unsigned long i=0; i < pList->NumOfParams; i++)
	{
		LOG(HELPERFUNC,"Print_SConfig_List: param count: %d",pList->NumOfParams);
		LOGW(HELPERFUNC,_T(" - 0x%08x (%s):  0x%08x (%d)"),pList->ConfigPtr[i].Parameter, dbug_param2str(pList->ConfigPtr[i].Parameter), pList->ConfigPtr[i].Value, pList->ConfigPtr[i].Value);
	}
}


unsigned long GetTime()
	{
	FILETIME t;
	GetSystemTimeAsFileTime(&t);
	return t.dwLowDateTime;
	}

int convert_ascii_char_to_nibble(char c)
{
	if ((c >= '0') && (c <= '9'))
		return c - '0';
	if ((c >= 'A') && (c <= 'F'))
		return 10 + c - 'A';
	if ((c >= 'a') && (c <= 'f'))
		return 10 + c - 'a';
	return 16; // in case of error
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int convert_hex_to_int( char * ascii, int len )
  {
  int nibble;
  int index=0;
  unsigned long number = 0;
  while ( ascii[index] && (index<len) && ((nibble=convert_ascii_char_to_nibble(ascii[index]))!=16) )
    {
    number *= 16;
    number += nibble;
    index++;  
    }
  if (nibble==16)
    return -1;
  return number;
  }

