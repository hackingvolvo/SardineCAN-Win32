#include "stdafx.h"
#include <Windows.h>
#include "Arduino.h"
#include "registry.h"
#include "helper.h"

namespace Comm {

	HANDLE ghInitCompleteEvent;	// signaled to other threads when arduino is connected
	HANDLE ghRequestInitEvent;	// other threads can request init, if first one failed.
	HANDLE ghCommEvent;			// comm event (bytes available in read buffer, com error etc.)
	HANDLE ghCommExitEvent;		// request for exiting, issued by main thread
	HANDLE ghCommExitedEvent;
	HANDLE waitHandles[10];
	int waitHandleCount=0;

	HANDLE  commThread=NULL;

	void SetInitialized()
	{
		LOG(HELPERFUNC,"Comm::SetInitialized");
		SetEvent(ghInitCompleteEvent);
	}

	void RequestInitialization()
	{
		LOG(HELPERFUNC,"Comm::RequestInitialization");
		SetEvent(ghRequestInitEvent);
	}

	bool WaitUntilInitialized( unsigned long timeout )
	{
		LOG(MAINFUNC,"Comm:::WaitUntilInitialized - timeout %d",timeout);
		if (Arduino::IsConnected())
		{
			return true;
		}
		else
		{
			RequestInitialization();
			DWORD dwWaitResult;
			dwWaitResult= WaitForSingleObject( 
				ghInitCompleteEvent, // event handle
				timeout);    

			if (dwWaitResult!=WAIT_OBJECT_0)
			{
				LOG(ERR,"Comm::WaitUntilInitialized - WaitForSingleObject failed (%d)", GetLastError());
				return false;
			}
			LOG(MAINFUNC,"Comm::WaitUntilInitialized - Initialized!");
		}
		return true;
	}

	bool OpenArduino()
	{
		int com_port = -1;
		int baud_rate = -1;
		int disable_DTR = -1;
		SardineRegistry::GetSettingsFromRegistry( &com_port, &baud_rate, &disable_DTR );
		if (Arduino::OpenDevice(com_port,baud_rate, (disable_DTR==1))==0)
		{
			Arduino::Listen(ghCommEvent);
			// send ok signal to processes that are waiting for initialization to end
			LOG(MAINFUNC,"Comm::OpenArduino - signaling other threads that we are ready");
			SetInitialized();
			return true;
		} 
		return false;
	}

	bool WaitForEvents()
	{
		LOG(MAINFUNC,"Comm::WaitForEvents - blocking until events occur");
		DWORD ret;
		ret= WaitForMultipleObjects(waitHandleCount,waitHandles,false,INFINITE);
		if (ret==(WAIT_OBJECT_0+0))
		{
			LOG(MAINFUNC,"Comm::WaitForEvents - comm event");
			Arduino::HandleCommEvent();
			ResetEvent(ghCommEvent);
			Arduino::Listen(ghCommEvent);
		}
		else if (ret==(WAIT_OBJECT_0+1))
		{
			LOG(MAINFUNC,"Comm::WaitForEvents - 'request for initialization' event");
			OpenArduino();
			ResetEvent(ghRequestInitEvent);
		} 
		else if (ret==(WAIT_OBJECT_0+2))
		{
			LOG(MAINFUNC,"Comm::WaitForEvents - 'request for exit' event");
			return false;
		} else
		{
			LOG(MAINFUNC,"Comm::WaitForEvents - error in WaitForMultipleObjects: %d",ret=GetLastError());
			return false;
			}
		return true;
	}

	bool CreateEvents()
	{
		if ((ghInitCompleteEvent = CreateEvent( 
			NULL,               // default security attributes
			TRUE,               // manual-reset event
			FALSE,              // initial state is nonsignaled
			NULL // object name
			)) == NULL) 
		{ 
			LOG(ERR,"Comm::CreateEvents - Create 'InitComplete Event' failed (%d)\n", GetLastError());
			return false;
		}

		// create event for comm 
		if ((ghCommEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
		{
			LOG(ERR,"Comm::CreateEvents - Create 'Comm Event' failed (err %d); abort!", GetLastError());
			return false;
		}

		// create event for comm  exit
		if ((ghCommExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
		{
			LOG(ERR,"Comm::CreateEvents - Create 'Comm Exit event' failed (err %d); abort!", GetLastError());
			return false;
		}
		// create event for comm  exited
		if ((ghCommExitedEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
		{
			LOG(ERR,"Comm::CreateEvents - Create 'Comm Exited event' failed (err %d); abort!", GetLastError());
			return false;
		}

		// create event for request init 
		if ((ghRequestInitEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
		{
			LOG(ERR,"Comm::CreateEvents - Create 'Request Init' Event failed (err %d); abort!", GetLastError());
			return false;
		}
		waitHandles[0] = ghCommEvent;
		waitHandles[1] = ghRequestInitEvent;
		waitHandles[2] = ghCommExitEvent;
		waitHandleCount=3;
		return true;
	}

	void CloseEvents()
	{
		CloseHandle(ghCommEvent);
		CloseHandle(ghCommExitEvent);
		CloseHandle(ghRequestInitEvent);
		CloseHandle(ghCommExitedEvent);
	}


	DWORD WINAPI CommMainFunc()
	{
		LOG(INIT,"Comm::CommMainFunc");
		int exit=0;
		while (!exit)
		{
			if (!WaitForEvents())
				exit=1;
		}

		return 1;
	}


	DWORD WINAPI StartComm( LPVOID lpParam )
	{
		LOG(INIT,"Comm::StartComm");

		/*
		ghCommEvent = CreateEvent( 
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT("SardineCommEvent")  // object name
		); 

		if (ghCommEvent == NULL) 
		{ 
		LOG(ERR,"Comm::StartComm - CreateEvent failed (%d)\n", GetLastError());
		return false;
		}
		*/
		int ret = OpenArduino();
		CommMainFunc();
		Arduino::CloseDevice();
		LOG(INIT,"Comm::StartComm: Exiting..");
		SetEvent(ghCommExitedEvent);
		return 0;
	}




	bool createCommThread()
	{
		if (commThread==NULL)
		{
			LOG(INIT,"Comm::createCommThread - Creating init event object");
			if (!CreateEvents())
			{
				LOG(ERR,"Comm: Creating event objects failed!");
				return false;
			}

			LOG(INIT,"Comm::createCommThread - Creating thread");
			commThread = CreateThread( 
				NULL,                   // default security attributes
				0,                      // use default stack size  
				StartComm,				// thread function name
				NULL, /*pDataArray[i],*/          // argument to thread function 
				0,                      // use default creation flags 
				NULL); //&dwThreadIdArray[i]);   // returns the thread identifier 
			if (commThread!=NULL)
			{
				LOG(INIT,"Comm::createCommThread - Creating thread success");
			}
			else
			{
				LOG(ERR,"Comm::createCommThread - Creating thread FAILED!");
				return false;
			}
		}
		return true;
	}



	bool closeCommThread()
	{
		LOG(HELPERFUNC,"Comm::closeCommThread");
		SetEvent(ghCommExitEvent);
		WaitForSingleObject(ghCommExitedEvent,5000);
		CloseEvents();
		CloseHandle(commThread);
		return true;
	}

}