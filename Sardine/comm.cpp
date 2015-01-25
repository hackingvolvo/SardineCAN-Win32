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
#include <Windows.h>
#include "Arduino.h"
#include "registry.h"
#include "helper.h"

namespace Comm {

	HANDLE ghInitReqCompleteEvent;	// signaled to other threads when arduino initialization ended (either succeeded or failed)
	HANDLE ghRequestInitEvent;	// other threads can request init, if first one failed.
	HANDLE ghCommEvent;			// comm event (bytes available in read buffer, com error etc.)
	HANDLE ghCommExitEvent;		// request for exiting, issued by main thread
	HANDLE ghCommExitedEvent;
	HANDLE waitHandles[10];
	int waitHandleCount=0;

	int arduinoInitErrorCode;
	char ErrorMsg[80];

	char RequestedDeviceName[256];
	unsigned long deviceId;

	HANDLE  commThread=NULL;

	bool IsConnected()
	{
		return Arduino::IsConnected();
	}

	const char * GetCommErrorMsg()
	{
		return &ErrorMsg[0];
	}

	unsigned long GetDeviceId()
	{
		return deviceId;
	}

	void SetInitReqComplete()
	{
		LOG(HELPERFUNC,"Comm::SetInitReqComplete");
		SetEvent(ghInitReqCompleteEvent);
	}

	void RequestInitialization( const char * deviceName )
	{
		LOG(HELPERFUNC,"Comm::RequestInitialization");
		if (deviceName)
		{
		if (strlen(deviceName) < 256 )
			strcpy_s(RequestedDeviceName, 256, deviceName);
		else
			strncpy_s(RequestedDeviceName, deviceName,256);
		} else
			strcpy_s(RequestedDeviceName,256,"");
		SetEvent(ghRequestInitEvent);
	}

	int WaitUntilInitialized( const char * deviceName, unsigned long timeout )
	{
		LOG(MAINFUNC,"Comm:::WaitUntilInitialized - timeout %d",timeout);
		if (Arduino::IsConnected())
		{
			return STATUS_NOERROR;
		}
		else
		{
			RequestInitialization(deviceName);
			DWORD dwWaitResult;
			dwWaitResult= WaitForSingleObject( 
				ghInitReqCompleteEvent, // event handle
				timeout);    

			if (dwWaitResult==WAIT_TIMEOUT)
			{
				strcpy_s(ErrorMsg,80,"Init timeout!");
				return ERR_FAILED;
			} else
			if (dwWaitResult!=WAIT_OBJECT_0)
			{
				LOG(ERR,"Comm::WaitUntilInitialized - WaitForSingleObject failed (%d)", GetLastError());
				strcpy_s(ErrorMsg,80,"Error in wait object!");
				return ERR_FAILED;
			}
			if (arduinoInitErrorCode!=STATUS_NOERROR)
			{
				LOG(MAINFUNC,"Comm::WaitUntilInitialized - Init failed! (%d)",arduinoInitErrorCode);
				return arduinoInitErrorCode;
			} else
			{
				LOG(MAINFUNC,"Comm::WaitUntilInitialized - Initialized!");
			}
		}
		return STATUS_NOERROR;
	}

	int OpenArduino()
	{
		int com_port = -1;
		int baud_rate = -1;
		int disable_DTR = -1;
		deviceId = -1;
		if (SardineRegistry::GetSettingsFromRegistry( RequestedDeviceName, &com_port, &baud_rate, &disable_DTR, &deviceId ))
		{
			unsigned int ret = Arduino::OpenDevice(com_port,baud_rate, (disable_DTR==1));
			switch(ret)
			{
				case ARDUINO_INIT_OK:
					{
					Arduino::Listen(ghCommEvent);
					return STATUS_NOERROR;
					}
					break;
				case ARDUINO_ALREADY_CONNECTED:
					{
					// ignore
					return STATUS_NOERROR;
					}
					break;
				case ARDUINO_IN_USE:
					{
					LOG(MAINFUNC,"Comm::OpenArduino - Already in use!");
					return ERR_DEVICE_IN_USE;
					}
					break;
				case ARDUINO_OPEN_FAILED:
					{
					LOG(MAINFUNC,"Comm::OpenArduino - not connected!");
					return ERR_DEVICE_NOT_CONNECTED;
					}
					break;
				case ARDUINO_GET_COMMSTATE_FAILED:
				case ARDUINO_SET_COMMSTATE_FAILED:
				case ARDUINO_SET_COMMMASK_FAILED:
				case ARDUINO_CREATE_EVENT_FAILED:
					{
					LOG(MAINFUNC,"Comm::OpenArduino - get/set comm state/mask / create event failed!");
					strcpy_s(ErrorMsg,80,"Get/set comm state/mask or create event failed!");
					return ERR_FAILED;
					}
					break;
				default:
					LOG(MAINFUNC,"Comm::OpenArduino - invalid state!");
					strcpy_s(ErrorMsg,80,"Invalid state!");
					return ERR_FAILED;
					break;
			}

		} 

		LOG(MAINFUNC,"Comm::OpenArduino - Not found in registry!");
		strcpy_s(ErrorMsg,80,"Device not found in registry!");
		return ERR_FAILED;

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
			arduinoInitErrorCode = OpenArduino();
			// send ok signal to processes that are waiting for initialization to end
			SetInitReqComplete();
			LOG(MAINFUNC,"Comm::OpenArduino - signaling other threads that we are ready");
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
		if ((ghInitReqCompleteEvent = CreateEvent( 
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
		// do not open arduino by default
//		int ret = OpenArduino();

		CommMainFunc();

		Arduino::CloseDevice();
		LOG(INIT,"Comm::StartComm: Exiting..");
		SetEvent(ghCommExitedEvent);
		return 0;
	}




	bool CreateCommThread()
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



	bool CloseCommThread()
	{
		LOG(HELPERFUNC,"Comm::closeCommThread");
		SetEvent(ghCommExitEvent);
		WaitForSingleObject(ghCommExitedEvent,5000);
		CloseEvents();
		CloseHandle(commThread);
		return true;
	}

}