// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "Sardine.h"
#include "helper.h"
#include "comm.h"

bool setup()
{
	LOG(INIT,"Sardine: setup");

   // Send all reports to STDOUT
   _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
   _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
   _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
   _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
   _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
   _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
      SET_CRT_DEBUG_FIELD( _CRTDBG_DELAY_FREE_MEM_DF );

	return Comm::createCommThread();
}

void exitdll()
{
	LOG(INIT,"Sardine: Exitdll");
	Comm::closeCommThread();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	LOG(INIT,"Sardine v1.35 : (c) Olaf @ HackingVolvo Labs 2012");
	LOG(INIT,"DllMain: %d",ul_reason_for_call);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!setup())
			return FALSE;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		exitdll();
		break;
	}
	LOG(INIT,"Sardine: DllMain exit with return TRUE");
	return TRUE;
}

