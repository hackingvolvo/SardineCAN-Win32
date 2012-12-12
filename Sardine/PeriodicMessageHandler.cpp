#include "stdafx.h"
#include "helper.h"
#include "PeriodicMessageHandler.h"

CPeriodicMessageHandler::CPeriodicMessageHandler(void)
{
	pMsgNum = 0;
	hMsgHandlerThread = NULL;
	stopped=0;
}


CPeriodicMessageHandler::~CPeriodicMessageHandler(void)
{
	RemoveAllPeriodicMessages();

	// notify the other thread to exit gracefully
	LOG(MAINFUNC,"CPeriodicMessageHandler::~CPeriodicMessageHandler: Signaling child thread to exit");
	stopped=1;
	SetEvent(hMsgHandlerEvent);

	CloseHandle(hMsgHandlerThread);
	CloseHandle(hMsgHandlerEvent);
}

DWORD WINAPI MainMsgHandlerThreadFunction( LPVOID lpParam )
{
	LOG(MAINFUNC,"CPeriodicMessageHandler::MainThreadFunction");

	CPeriodicMessageHandler * me = (CPeriodicMessageHandler*)lpParam;

	// loop until there's error
	while (me->HandleSignal())
		{
		}
	LOG(INIT,"CPeriodicMessageHandler::MainThreadFunction - exiting");
	return 0;
}


bool CPeriodicMessageHandler::HandleSignal()
{
	DWORD ret;
	LOG(HELPERFUNC,"CPeriodicMessageHandler::HandleSignal - Waiting for signal...");

	ret= WaitForSingleObjectEx(hMsgHandlerEvent,INFINITE,true);
	if (ret==WAIT_OBJECT_0)
	{
		LOG(HELPERFUNC,"CPeriodicMessageHandler::HandleSignal - Signaled!");
		if (stopped)
			{
			// owner thread signaled us to stop
			return false;
			}
		HandlerLock.Lock();
		for (int i=0;i<pMsgNum;i++)
		{
			if (!periodicMessages[i]->HasTimer())
			{
				LOG(HELPERFUNC,"CPeriodicMessageHandler::HandleSignal - Attaching timer to new periodic message #%d",i);
				periodicMessages[i]->CreateTimer();
					// timer handle
			}
		}

		// reset the event, so that WaitForSingleObject will wait again for next event signal
		ResetEvent(hMsgHandlerEvent);

		HandlerLock.Unlock();
	} else
	if (ret==STATUS_USER_APC)
	{
//		LOG(HELPERFUNC,"CPeriodicMessageHandler::HandleSignal - one of the timers callbacks were called");
	} else
	{
		LOG(ERR,"CPeriodicMessageHandler::HandleSignal - Error in WaitForSingleObject: %d (ret=%d), (stopped=%d)",GetLastError(),ret,stopped);
		return false;
	}
	return true;
}

bool CPeriodicMessageHandler::createMsgHandlerThread(int channelId)
{
	if (hMsgHandlerThread==NULL)
	{
		LOG(MAINFUNC,"CPeriodicMessageHandler::createMsgHandlerhread - Creating init event object");

		// create unique event name based on channel id
//		char eventName[64];
//		sprintf_s(eventName,64,"SardineMsgHandlerEvent%x",channelId);

		hMsgHandlerEvent = CreateEventA( 
			NULL,		// default security attributes
			TRUE,		// manual-reset event
			FALSE,		// initial state is nonsignaled
			NULL);		// object name

		if (hMsgHandlerEvent == NULL) 
		{ 
			LOG(ERR,"CPeriodicMessageHandler::createMsgHandlerThread - CreateEvent failed (%d)\n", GetLastError());
			return false;
		}

		LOG(MAINFUNC,"CPeriodicMessageHandler::createMsgHandlerhread - Creating thread");
		hMsgHandlerThread = CreateThread( 
			NULL,                   // default security attributes
			0,                      // use default stack size  
			MainMsgHandlerThreadFunction,       // thread function name
			(void*)this, /*pDataArray[i],*/          // argument to thread function 
			0,                      // use default creation flags 
			NULL); //&dwThreadIdArray[i]);   // returns the thread identifier 
		if (hMsgHandlerThread!=NULL)
		{
			LOG(MAINFUNC,"CPeriodicMessageHandler::createMsgHandlerhread - Creating thread success");
		}
		else
		{
			LOG(ERR,"CPeriodicMessageHandler::createMsgHandlerhread - Creating thread FAILED!");
			return false;
		}
	}
	return true;
}


int CPeriodicMessageHandler::AddPeriodicMessage(CPeriodicMsg * msg)
{
	LOG(HELPERFUNC,"CPeriodicMessageHandler::AddPeriodicMessage - msg id 0x%x",msg->Id());
	HandlerLock.Lock();
	if (pMsgNum>=MAX_PERIODIC_MSGS)
	{
		LOG(HELPERFUNC,"CPeriodicMessageHandler::AddPeriodicMessage - too many periodic messages!");
		HandlerLock.Unlock();
		return ERR_EXCEEDED_LIMIT;
	}

	periodicMessages[pMsgNum] = msg;
	pMsgNum++;

	// notify our handling thread that we have new periodic message. 
	SetEvent(hMsgHandlerEvent);

	HandlerLock.Unlock();
	return STATUS_NOERROR;
}


void CPeriodicMessageHandler::RemoveAllPeriodicMessages()
{
	HandlerLock.Lock();
	for (int i=0;i<pMsgNum;i++)
	{
		delete periodicMessages[i];
	}
	pMsgNum=0;
	HandlerLock.Unlock();
}


int CPeriodicMessageHandler::RemovePeriodicMessage(unsigned long Id)
{
	LOG(HELPERFUNC,"CPeriodicMessageHandler::RemovePeriodicMessage - msg id 0x%x",Id);
	HandlerLock.Lock();
	int i=0;
	while ( (i<pMsgNum) && (periodicMessages[i]->Id() != Id) )
		i++;
	if (i==pMsgNum)
	{
		HandlerLock.Unlock();
		LOG(ERR,"CPeriodicMessageHandler::RemovePeriodicMessage - didn't find msg with id 0x%x",Id);
		return ERR_INVALID_MSG_ID;
	}
	delete periodicMessages[i];
	for (;i<pMsgNum-1;i++)
		periodicMessages[i] = periodicMessages[i+1];
	pMsgNum--;
	HandlerLock.Unlock();
	return STATUS_NOERROR;
}
