#include "stdafx.h"
#include "helper.h"
#include "../Sardine/sardine_defs.h"

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

int ParseReturnValue( long int ret )
	{
	int i=0;
	while (J2534_error_msgs[i].msg!=NULL) {
		if (ret==J2534_error_msgs[i].err_id)
			{
			LOG("Error [%d]: %s",ret,J2534_error_msgs[i].msg);
			return ret;
			}
		i++;
	}
	LOG("ParseReturnValue: Invalid error value! (%d)",ret);
	return -1;
	}

void PrintError( int error )
{
	LPTSTR errorText = NULL;
	FormatMessage(
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
	   (LPTSTR)&errorText,  // output 
	   0, // minimum size for output buffer
	   NULL);   // arguments - see note
	if (errorText != NULL)
	{
		_tprintf(TEXT("Error: [%d] %s\n"), error, errorText);
	}
}