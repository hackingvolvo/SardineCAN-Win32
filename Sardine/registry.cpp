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

#include "stdafx.h"
#include <malloc.h>
#include "helper.h"

namespace SardineRegistry {

	int Registry_GetString(HKEY hKey, LPCTSTR szValueName, LPTSTR * lpszResult) {
		DWORD dwType=0, dwDataSize=0, dwBufSize=0;
		int res;

		if (lpszResult != NULL) *lpszResult = NULL;
		if (hKey == NULL || lpszResult == NULL) return E_INVALIDARG;

		// get value size
		res = RegQueryValueEx(hKey, szValueName, 0, &dwType, NULL, &dwDataSize );
		if (res != ERROR_SUCCESS)
			return res;

		// check the type
		if (dwType != REG_SZ) 
			return ERROR_DATATYPE_MISMATCH;

		dwBufSize = dwDataSize + (1 * sizeof(TCHAR));
		*lpszResult = (LPTSTR)malloc(dwBufSize);
		if (*lpszResult == NULL) 
			return ERROR_NOT_ENOUGH_MEMORY;

		// get the value
		res = RegQueryValueEx(hKey, szValueName, 0, &dwType, (LPBYTE) *lpszResult, &dwDataSize );
		if (res != ERROR_SUCCESS) 
		{ 
			free(*lpszResult); 
			return res;
		}
		(*lpszResult)[(dwBufSize / sizeof(TCHAR)) - 1] = TEXT('\0');

		return ERROR_SUCCESS;
	}


	// =====================================================================================
	int Registry_GetDWord(HKEY hKey, LPCTSTR szValueName, DWORD * lpdwResult) {

		int res;
		DWORD dwDataSize = sizeof(DWORD);
		DWORD dwType = 0;

		if (hKey == NULL || lpdwResult == NULL) return E_INVALIDARG;
		res = RegQueryValueEx(hKey, szValueName, 0, &dwType, (LPBYTE) lpdwResult, &dwDataSize );
		if (res != ERROR_SUCCESS) 
			return res;
		else 
			if (dwType != REG_DWORD) return ERROR_DATATYPE_MISMATCH;

		return ERROR_SUCCESS;
	}


	// Get settings from Registry: If a value cannot be found in registry, original values (referenced with parameters) will not be altered

	bool GetSettingsFromRegistry( int * ComPort, int * BaudRate, int * disableDTR )
	{
		HKEY hKeySoftware, hKeyPTS0404, hKeySardineCAN; 
		//	DWORD KeyType, KeySize;
		DWORD dwVal;

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software"), 0, KEY_READ, &hKeySoftware) != ERROR_SUCCESS)
		{
			LOG(ERR,"GetSettingsFromRegistry: Cannot open registry key: SOFTWARE");
			return FALSE;
		}
		if (RegOpenKeyEx(hKeySoftware, L"PassThruSupport.04.04", 0, KEY_READ, &hKeyPTS0404) != ERROR_SUCCESS)
		{
			LOG(ERR,"GetSettingsFromRegistry: Cannot open registry key: PassThruSupport.04.04!");
			RegCloseKey(hKeySoftware);
			return FALSE;
		}
		RegCloseKey(hKeySoftware);

		if (RegOpenKeyEx(hKeyPTS0404, L"Sardine CAN", 0, KEY_READ , &hKeySardineCAN) != ERROR_SUCCESS)
		{
			LOG(ERR,"GetSettingsFromRegistry: Couldn't find our Sardine CAN entry in registry!");
			RegCloseKey(hKeyPTS0404);
			return FALSE;
		}
		RegCloseKey(hKeyPTS0404);

		if (Registry_GetDWord(hKeySardineCAN,TEXT("COM_PORT"),&dwVal) != ERROR_SUCCESS)
		{
			LOG(ERR,"GetSettingsFromRegistry: No Com port entry!");
		}
		else
			*ComPort = dwVal;

		if (Registry_GetDWord(hKeySardineCAN,TEXT("BAUD_RATE"),&dwVal) != ERROR_SUCCESS)
		{
			LOG(ERR,"GetSettingsFromRegistry: No baud rate entry!");
		}
		else
			*BaudRate = dwVal;

		if (Registry_GetDWord(hKeySardineCAN,TEXT("DISABLE_DTR"),&dwVal) != ERROR_SUCCESS)
		{
			LOG(ERR,"GetSettingsFromRegistry: No disableDTR entry!");
		}
		else
			*disableDTR = dwVal;


		RegCloseKey(hKeySardineCAN);
		return true;
	}



	bool GetValueFromRegistry( HKEY previousKey, TCHAR * valueName, unsigned long * value )
	{
		HKEY hKeySoftware, hKeyPTS0404, hKeySardineCAN; 
		//	DWORD KeyType, KeySize;
		DWORD dwVal;
		DWORD ret;

		if (previousKey==NULL)
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software"), 0, KEY_READ, &hKeySoftware) != ERROR_SUCCESS)
			{
				LOG(ERR,"GetSettingsFromRegistry: Cannot open registry key: SOFTWARE");
				return FALSE;
			}
			if (RegOpenKeyEx(hKeySoftware, L"PassThruSupport.04.04", 0, KEY_READ, &hKeyPTS0404) != ERROR_SUCCESS)
			{
				LOG(ERR,"GetSettingsFromRegistry: Cannot open registry key: PassThruSupport.04.04!");
				RegCloseKey(hKeySoftware);
				return FALSE;
			}
			RegCloseKey(hKeySoftware);

			if (RegOpenKeyEx(hKeyPTS0404, L"Sardine CAN", 0, KEY_READ, &hKeySardineCAN) != ERROR_SUCCESS)
			{
				LOG(ERR,"GetSettingsFromRegistry: Couldn't find our Sardine CAN entry in registry!");
				RegCloseKey(hKeyPTS0404);
				return FALSE;
			}
			RegCloseKey(hKeyPTS0404);
		} else
			hKeySardineCAN=previousKey;

		ret=Registry_GetDWord(hKeySardineCAN,valueName,&dwVal);
		if (previousKey==NULL)
			RegCloseKey(hKeySardineCAN);

		if (ret != ERROR_SUCCESS)
		{
			LOGW(ERR,L"GetSettingsFromRegistry: No %s entry!",valueName);
			return false;
		}
		else
			*value = dwVal;
		return true;
	}

}