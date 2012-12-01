#pragma once

#include <fstream>
#include <iostream>
#include <Windows.h>
#include "sardine_defs.h"
#include "shim_debug.h"

// The following macros set and clear, respectively, given bits
// of the C runtime library debug flag, as specified by a bitmask.
#include <crtdbg.h>
#ifdef   _DEBUG
#define  SET_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  CLEAR_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
#define  SET_CRT_DEBUG_FIELD(a)   ((void) 0)
#define  CLEAR_CRT_DEBUG_FIELD(a) ((void) 0)
#endif

#define ENABLE_LOGGING
#define ENABLE_MSG_LOGGING

//#define STACK_INDENTING

#ifdef ENABLE_LOGGING


// debug fields. 
// God damn, wasted few minutes of my life figuring out what's wrong, only to realize binary literals don't exist in Visual C++...
#define DISABLED				0	// 0b0000000000
#define ERR						1	// 0b0000000001
#define	INIT					2	// 0b0000000010
#define MAINFUNC				4	// 0b0000000100
#define PROTOCOL				8	// 0b0000001000
#define HELPERFUNC				16	// 0b0000010000
#define ARDUINO_MSG				32	// 0b0000100000
#define PROTOCOL_MSG			64	// 0b0001000000
#define PROTOCOL_VERBOSE		128	// 0b0010000000
#define ARDUINO_MSG_VERBOSE		256	// 0b0100000000
#define PROTOCOL_MSG_VERBOSE	512	// 0b1000000000

namespace debug {
	// FIXME: load default values from Windows registry, and add DLL functions to change these settings
	extern unsigned long debug_fields;
}


	typedef enum { UNDEFINED, RECEIVED, SENT, LOOP_BACK, ISO15765_RECV, ISO15765_SENT } LogMessageType; 
	void LogMessage( PASSTHRU_MSG * pMsg, LogMessageType msgType, unsigned long channelId, char * comment );


int get_stack_trace_depth();

#ifdef STACK_INDENTING
#define INDENT_AND_DECORATE(handle,debug_field) { \
	int sl = get_stack_trace_depth();	\
	if (sl>10)					\
		sl=10;					\
	for (int i=0;i<sl;i++)		\
		handle << "  ";			\
}
#else
#define INDENT_AND_DECORATE(handle,debug_field) { \
	if (debug_field==ERR)	\
		handle << "==>";	\
	if (debug_field>INIT)	\
		handle << " ";		\
	if (debug_field>MAINFUNC)	\
		handle << " ";		\
	if ( (debug_field==PROTOCOL_VERBOSE) \
		|| (debug_field==PROTOCOL_MSG_VERBOSE)	\
		|| (debug_field==ARDUINO_MSG_VERBOSE) )	\
		handle << " ";	\
}
#endif

#define LOG(debug_field,message, ...){	\
 if (debug::debug_fields & debug_field)	\
	{									\
	 char szMessageBuffer[1024] = {0};	\
	 SYSTEMTIME systemTime;				\
	 GetSystemTime( &systemTime );		\
	 sprintf_s(szMessageBuffer, 1024, "[%2d:%2d:%2d.%3d] ",systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds);\
	 std::ofstream handle;				\
	 handle.open("c:\\sardine\\sardine.log",std::ios_base::app); \
	 handle << szMessageBuffer;			\
	 INDENT_AND_DECORATE(handle,debug_field)	\
	 sprintf_s(szMessageBuffer, 1024, message, __VA_ARGS__);\
	 handle << szMessageBuffer;			\
	 handle << "\n" << std::flush;					\
	 }									\
}

#define LOG_BYTES(debug_field,count,bytes){	\
 if (debug::debug_fields & debug_field)	\
	{									\
	 char szMessageBuffer[64] = {0};	\
	 char szValBuffer[16] = {0};	\
	 SYSTEMTIME systemTime;				\
	 GetSystemTime( &systemTime );		\
	 sprintf_s(szMessageBuffer, 64, "[%2d:%2d:%2d.%3d] %d bytes: ",systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds,count);\
	 std::ofstream handle;				\
	 handle.open("c:\\sardine\\sardine.log",std::ios_base::app); \
	 handle << szMessageBuffer;			\
	 INDENT_AND_DECORATE(handle,debug_field)	\
	 for (unsigned int i=0;i<count;i++) {			\
		sprintf_s(szValBuffer,16,"%2x ",bytes[i]); \
		handle << szValBuffer;	}		\
	 handle << "\n" << std::flush;					\
	 }									\
}



#define LOGW( debug_field, message, ...){ \
 if (debug::debug_fields & debug_field)	\
	{									\
	 WCHAR szMessageBuffer[1024] = {0}; \
	 char szMessageBufferAsc[1024] = {0}; \
	 SYSTEMTIME systemTime; \
	 GetSystemTime( &systemTime ); \
	 sprintf_s(szMessageBufferAsc, 1024, "[%2d:%2d:%2d.%3d] ",systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds); \
	 std::ofstream handle;  \
	 handle.open("c:\\sardine\\sardine.log",std::ios_base::app);  \
	 handle << szMessageBufferAsc; \
	 swprintf_s(szMessageBuffer, 1024, message, __VA_ARGS__); \
	 char * ascbuffer = ConvertLPWSTRToLPSTR( szMessageBuffer ) ; \
	 handle << ascbuffer; \
	 delete ascbuffer; \
	 handle << "\n"; \
	 std::cout << "\n" << std::flush; \
	}					\
 }

#define  dtDebug(message, ...) LOGW(HELPERFUNC,message,__VA_ARGS__)


#else
#define LOG(message,...)
#define LOGT(message,...)
#define dtDebug(message,...)
#endif


void PrintError( int error );
char* ConvertLPWSTRToLPSTR (LPWSTR lpwszStrIn);

unsigned long GetTime();
int convert_ascii_char_to_nibble(char c);
unsigned int convert_hex_to_int( char * ascii, int len );
void Print_SConfig_List(SCONFIG_LIST *pList);
void Print_SByte_Array(SBYTE_ARRAY * pArray);
void Print_IOCtl_Cmd( unsigned long IoctlID );
//LPCTSTR dbug_param2str(unsigned long ParamID);

//void dtDebug(LPCTSTR format, ...);
