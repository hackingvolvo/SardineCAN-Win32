// SardineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "helper.h"
#include "../sardine/sardinedll.h"
#include "../sardine/sardine_defs.h"
#include "tests.h"

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME_LENGTH 255
#define MAX_VALUE_LENGTH 1024


#define LOAD_FUNCTION( name, type ) {\
	name = type(GetProcAddress(hDLL, #name )); \
	if (name==NULL)	\
		PRINT_ERROR_AND_EXIT\
	}					


	TCHAR DllLibraryPath[MAX_VALUE_LENGTH];

	/*
	** Define permanent storage for the handle to the J2534 DLL library.
	*/
	HINSTANCE hDLL;

	/*
	** Define permanent storage for all the PassThru function addresses.
	*/
	PTOPEN	PassThruOpen;
	PTCLOSE	PassThruClose;
	PTCONNECT PassThruConnect;
	PTDISCONNECT PassThruDisconnect;
	PTREADMSGS PassThruReadMsgs;
	PTWRITEMSGS PassThruWriteMsgs;
	PTSTARTPERIODICMSG PassThruStartPeriodicMsg;
	PTSTOPPERIODICMSG PassThruStopPeriodicMsg;
	PTSTARTMSGFILTER PassThruStartMsgFilter;
	PTSTOPMSGFILTER PassThruStopMsgFilter;
	PTSETPROGRAMMINGVOLTAGE PassThruSetProgrammingVoltage;
	PTREADVERSION PassThruReadVersion;
	PTGETLASTERROR PassThruGetLastError;
	PTIOCTL PassThruIoctl;


	DWORD ret;

	BOOL TraverseRegistry(void)
	{
		HKEY hKeySoftware, hKeyPTS0404, hKeySardineCAN; 
		DWORD KeyType, KeySize;

		TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
		DWORD    cbName;                   // size of name string 
		TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
		DWORD    cchClassName = MAX_PATH;  // size of class string 
		DWORD    cSubKeys=0;               // number of subkeys 
		DWORD    cbMaxSubKey;              // longest subkey size 
		DWORD    cchMaxClass;              // longest class string 
		DWORD    cValues;              // number of values for key 
		TCHAR	 achValueName[MAX_VALUE_NAME_LENGTH];
		BYTE	 achValue[MAX_VALUE_LENGTH];
		DWORD	 valueType;
		DWORD    cchMaxValue;          // longest value name 
		DWORD    cbMaxValueData;       // longest value data 
		DWORD    cbSecurityDescriptor; // size of security descriptor 
		FILETIME ftLastWriteTime;      // last write time 

		DWORD i; 


		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software"), 0, KEY_READ/*|KEY_WOW64_64KEY*/, &hKeySoftware) !=
			ERROR_SUCCESS)
		{
			LOG("Cannot open registry key: SOFTWARE!");
			return FALSE;
		}
		if (RegOpenKeyEx(hKeySoftware, L"PassThruSupport.04.04", 0, KEY_READ/*|KEY_WOW64_64KEY*/, &hKeyPTS0404) != ERROR_SUCCESS)
		{
			LOG("Cannot open registry key: PassThruSupport.04.04!");
			RegCloseKey(hKeySoftware);
			return FALSE;
		}
		RegCloseKey(hKeySoftware);

		ret = RegQueryInfoKey(
			hKeyPTS0404,                    // key handle 
			achClass,                // buffer for class name 
			&cchClassName,           // size of class string 
			NULL,                    // reserved 
			&cSubKeys,               // number of subkeys 
			&cbMaxSubKey,            // longest subkey size 
			&cchMaxClass,            // longest class string 
			&cValues,                // number of values for this key 
			&cchMaxValue,            // longest value name 
			&cbMaxValueData,         // longest value data 
			&cbSecurityDescriptor,   // security descriptor 
			&ftLastWriteTime);       // last write time 

		for (i=0; i<cSubKeys; i++) 
		{ 
			cbName = MAX_KEY_LENGTH;
			ret = RegEnumKeyEx(hKeyPTS0404, i,
				achKey, 
				&cbName, 
				NULL, 
				NULL, 
				NULL, 
				&ftLastWriteTime); 
			if (ret == ERROR_SUCCESS) 
			{
				LOGW(L"Found J2534 device: (%d) %s", i+1, achKey);
				//                _tprintf(TEXT("Found J2534 device: (%d) %s\n"), i+1, achKey);
			}
		}

		if (RegOpenKeyEx(hKeyPTS0404, L"Sardine CAN", 0, KEY_READ /*| KEY_WOW64_64KEY*/ , &hKeySardineCAN) != ERROR_SUCCESS)
		{
			LOG("Couldn't find our Sardine CAN entry in registry!");
			RegCloseKey(hKeyPTS0404);
			return FALSE;
		}
		RegCloseKey(hKeyPTS0404);

		cchClassName = MAX_PATH;
		ret = RegQueryInfoKey(
			hKeySardineCAN,                    // key handle 
			achClass,                // buffer for class name 
			&cchClassName,           // size of class string 
			NULL,                    // reserved 
			&cSubKeys,               // number of subkeys 
			&cbMaxSubKey,            // longest subkey size 
			&cchMaxClass,            // longest class string 
			&cValues,                // number of values for this key 
			&cchMaxValue,            // longest value name 
			&cbMaxValueData,         // longest value data 
			&cbSecurityDescriptor,   // security descriptor 
			&ftLastWriteTime);       // last write time 

		for (i=0; i<cValues; i++) 
		{ 
			cbName = MAX_KEY_LENGTH;
			cchMaxValue = MAX_VALUE_NAME_LENGTH;
			cbMaxValueData = MAX_VALUE_LENGTH;
			cchClassName = MAX_PATH;
			ret = RegEnumValue(hKeySardineCAN, i,
				achValueName, 
				&cchMaxValue, 
				NULL, 

				&valueType,
				achValue,
				&cbMaxValueData );
			if (ret == ERROR_SUCCESS) 
			{
				if ( (valueType==REG_SZ) || (valueType==REG_EXPAND_SZ) || (valueType==REG_LINK) )
				{
					LOGW(L"Sardine CAN registry: %d: [%s] = \"%s\"", i+1, achValueName,achValue);
				} else if (valueType==REG_DWORD)
				{
					DWORD val = (achValue[3]<<24) + (achValue[2]<<16) + (achValue[1]<<8) + achValue[0];	// little endian
					LOGW(L"Sardine CAN registry: %d: [%s] = [%d]", i+1, achValueName,val);
				} else {
					LOGW(L"Sardine CAN registry: %d: [%s] = *** currently unsupported type! ***", i+1, achValueName);
				} 
			}
		}


		KeySize=MAX_VALUE_LENGTH;
		if ( (ret=RegQueryValueEx(hKeySardineCAN, L"FunctionLibrary", 0, &KeyType, (LPBYTE)&DllLibraryPath, &KeySize)) !=
			ERROR_SUCCESS)
		{
			LOG("Couldn't find our Sardine CAN DLL path entry (FunctionLibrary) in registry!");
			RegCloseKey(hKeySardineCAN);
			return FALSE;
		}
		RegCloseKey(hKeySardineCAN);

		LOG("Loading library..");
		hDLL = LoadLibrary(DllLibraryPath);
		if (hDLL==NULL)
		{
			LOG("Loading failed, loading from reserve location.. (c:\\sardine\\sardine.dll)");
			hDLL = LoadLibrary(_T("c:\\sardine2\\sardine.dll"));
		}

		if (hDLL==NULL)
		{
			DWORD err = GetLastError();
			PrintError(err);
			return FALSE;
		}

		LOG("Loading functions..");

		LOG("Loading function #1..");
		LOAD_FUNCTION( PassThruOpen, (PTOPEN) );
		LOG("Loading function #2..");
		LOAD_FUNCTION( PassThruConnect, (PTCONNECT) );
		LOG("Loading function #3..");
		LOAD_FUNCTION( PassThruDisconnect, (PTDISCONNECT) );
		LOG("Loading function #4..");
		LOAD_FUNCTION( PassThruReadMsgs, (PTREADMSGS) );
		LOG("Loading function #5..");
		LOAD_FUNCTION( PassThruWriteMsgs, (PTWRITEMSGS) );
		LOG("Loading function #6..");
		LOAD_FUNCTION( PassThruStartPeriodicMsg, (PTSTARTPERIODICMSG) );
		LOG("Loading function #7..");
		LOAD_FUNCTION( PassThruStopPeriodicMsg, (PTSTOPPERIODICMSG) );
		LOG("Loading function #8..");
		LOAD_FUNCTION( PassThruStartMsgFilter, (PTSTARTMSGFILTER) );
		LOG("Loading function #9..");
		LOAD_FUNCTION( PassThruStopMsgFilter, (PTSTOPMSGFILTER) );
		LOG("Loading function #10..");
		LOAD_FUNCTION( PassThruSetProgrammingVoltage, (PTSETPROGRAMMINGVOLTAGE) );
		LOG("Loading function #11..");
		LOAD_FUNCTION( PassThruReadVersion, (PTREADVERSION) );
		LOG("Loading function #12..");
		LOAD_FUNCTION( PassThruGetLastError, (PTGETLASTERROR) );
		LOG("Loading function #13..");
		LOAD_FUNCTION( PassThruIoctl, (PTIOCTL) );
		LOG("Loading library&function: success! ");
		return TRUE;
	}



int _tmain(int argc, _TCHAR* argv[])
{
	int connect_failed=0,read_failed=0,write_failed=0,ioctl_failed=0,other_failed=0;
	LOG("Sardine test util v0.1 : (c) Olaf @ HackingVolvo Labs 2012");
	if (TraverseRegistry()==FALSE)
		{
		LOG("Loading library or functions failed!");
		return -1;
		}
	LOG("------------ Testing DLL API ------------ ");
//	PassThruOpen("Arduino Uno",&deviceId);

	connect_failed=ConnectTests();
	read_failed=ReadTests();
	write_failed=WriteTests();
	ioctl_failed=IOCTLTests();
	other_failed=OtherTests();

	LOG("Freeing library...");
	FreeLibrary(hDLL);
	LOG("\nConnect tests failed: %d\n Write tests failed: %d\n read tests failed: %d\n ioctl tests failed: %d\n other tests failed: %d\n",
		connect_failed,read_failed,write_failed,ioctl_failed,other_failed);
	getchar();

	return 0;
}

