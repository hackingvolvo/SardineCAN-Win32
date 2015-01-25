/*
**
** Copyright (C) 2012 Olaf @ Hacking Volvo blog (hackingvolvo.blogspot.com)
** Author: Olaf <hackingvolvo@gmail.com>
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

#include <fstream>
#include <iostream>
#include <Windows.h>
#include "sardine_defs.h"
#include "shim_debug.h"

#define ENABLE_LOGGING
#define ENABLE_MSG_LOGGING

#ifdef ENABLE_LOGGING

#define SARDINE_LOG_FILE "c:\\sardine\\sardine.log"
#define SARDINE_MSG_LOG_FILE "c:\\sardine\\sardine-msg.log"

// debug fields. 
// God damn, wasted few minutes of my life figuring out what's wrong, only to realize that binary literals don't exist in Visual C++...
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


#define LOG(debug_field,message, ...){	\
	if (debug::debug_fields & debug_field)	\
	{									\
		char szMessageBuffer[1024] = {0};	\
		SYSTEMTIME systemTime;				\
		GetSystemTime( &systemTime );		\
		sprintf_s(szMessageBuffer, 1024, "[%2d:%2d:%2d.%3d] ",systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds);\
		std::ofstream handle;				\
		try { \
		handle.open(SARDINE_LOG_FILE,std::ios_base::app); \
		handle << szMessageBuffer;			\
		INDENT_AND_DECORATE(handle,debug_field)	\
		sprintf_s(szMessageBuffer, 1024, message, __VA_ARGS__);\
		handle << szMessageBuffer;			\
		handle << "\n" << std::flush;					\
		handle.close(); \
		} \
		catch(std::ofstream::failure e) {}\
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
		try { \
			handle.open(SARDINE_LOG_FILE,std::ios_base::app); \
			handle << szMessageBuffer;			\
			INDENT_AND_DECORATE(handle,debug_field)	\
			for (unsigned int i=0;i<count;i++) {			\
			sprintf_s(szValBuffer,16,"%2x ",bytes[i]); \
			handle << szValBuffer;	}		\
			handle << "\n" << std::flush;					\
			handle.close(); \
		} \
		catch(std::ofstream::failure e) {}\
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
		try { \
			handle.open(SARDINE_LOG_FILE,std::ios_base::app);  \
			handle << szMessageBufferAsc; \
			swprintf_s(szMessageBuffer, 1024, message, __VA_ARGS__); \
			char * ascbuffer = ConvertLPWSTRToLPSTR( szMessageBuffer ) ; \
			handle << ascbuffer; \
			delete ascbuffer; \
			handle << "\n" << std::flush; \
			handle.close(); \
		} \
		catch(std::ofstream::failure e) {}\
	}\
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

void SetLastErrorMsg( const char * msg );