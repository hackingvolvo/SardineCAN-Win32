/*
**
** Copyright (C) 2012 Olaf @ Hacking Volvo blog (hackingvolvo.blogspot.com)
** Author: Olaf <hackingvolvo@gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation, either version 3 of the License, or (at
** your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this program; if not, <http://www.gnu.org/licenses/>.
**
*/

#pragma once
#include <fstream>
#include <iostream>


#define LOG(message, ...){\
 char szMessageBuffer[2048] = {0};\
 SYSTEMTIME systemTime;\
 GetSystemTime( &systemTime );\
 sprintf_s(szMessageBuffer, 2048, "[%2d:%2d:%2d.%3d] ",systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds);\
 std::ofstream handle; \
 handle.open("c:\\sardine\\sardinetest.log",std::ios_base::app); \
 handle << szMessageBuffer; \
 std::cout << szMessageBuffer; \
 sprintf_s(szMessageBuffer, 2048, message, __VA_ARGS__);\
 handle << szMessageBuffer; \
 std::cout << szMessageBuffer; \
 handle << "\n"; \
 std::cout << "\n"; \
 }


char* ConvertLPWSTRToLPSTR (LPWSTR lpwszStrIn);


#define LOGW( message, ...){ \
 WCHAR szMessageBuffer[2048] = {0}; \
 char szMessageBufferAsc[2048] = {0}; \
 SYSTEMTIME systemTime; \
 GetSystemTime( &systemTime ); \
 sprintf_s(szMessageBufferAsc, 2048, "[%2d:%2d:%2d.%3d] ",systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds); \
 std::ofstream handle;  \
 handle.open("c:\\sardine\\sardinetest.log",std::ios_base::app);  \
 handle << szMessageBufferAsc; \
 std::cout << szMessageBufferAsc; \
 swprintf_s(szMessageBuffer, 2048, message, __VA_ARGS__); \
 char * ascbuffer = ConvertLPWSTRToLPSTR( szMessageBuffer ) ; \
 handle << ascbuffer; \
 std::cout << ascbuffer; \
 delete ascbuffer; \
 handle << "\n"; \
 std::cout << "\n"; \
 }

int ParseReturnValue( long int ret );

void PrintError( int error );

#define PRINT_ERROR_AND_EXIT \
		{ \
		DWORD err = GetLastError(); \
		PrintError(err); \
		return FALSE;\
		}

