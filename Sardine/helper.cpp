#include "stdafx.h"
#include "helper.h"
#include "sardine_defs.h"
#include "shim_debug.h"
#include <stdio.h>

namespace debug {
	// FIXME: load default values from Windows registry, and add DLL functions to change these settings
	unsigned long debug_fields = ERR | INIT | MAINFUNC | HELPERFUNC | ARDUINO_MSG | PROTOCOL | PROTOCOL_VERBOSE | PROTOCOL_MSG | PROTOCOL_MSG_VERBOSE | ARDUINO_MSG_VERBOSE;
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
	 handle.open(SARDINE_MSG_LOG_FILE,std::ios_base::app); 
	 handle << szMessageBuffer;

	 switch (msgType)
	 {
	 case LogMessageType::RECEIVED:
		 handle << ">>>>";
		 break;
	 case LogMessageType::SENT:
		 handle << "<<<<";
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


void Print_IOCtl_Cmd( unsigned long IoctlID )
{
	LOGW(HELPERFUNC,_T("IOCTL Command: %d [%s]"),IoctlID, dbug_ioctl2str(IoctlID));
}


void Print_SByte_Array(SBYTE_ARRAY * pArray)
	{
	if (pArray == NULL)
	{
		LOG(ERR,"Print_SByte_Array: pArray == NULL");
		return;
	}
	LOG_BYTES(HELPERFUNC, pArray->NumOfBytes, pArray->BytePtr);
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

