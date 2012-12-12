#include "stdafx.h"
#include "PeriodicMsg.h"
#include <Windows.h>
#include "helper.h"

   #define _SECOND 10000000
   #define _MILLISECOND 10000

#define FIRST_TIME_SIGNALING 50 // timer will be signaled 50ms first time after creating the timer. After that, every (x) milliseconds, where (x) is selected when creating timer


HANDLE CPeriodicMsg::Handle()
{
	return hTimer;
}

PASSTHRU_MSG * CloneMsg(PASSTHRU_MSG * pMsg)
{
	PASSTHRU_MSG * _msg = new PASSTHRU_MSG;
	if (_msg==NULL)
		return NULL;
	memcpy(_msg,pMsg,sizeof(PASSTHRU_MSG));
	return _msg;
}

VOID CALLBACK Timer(
	LPVOID lpArg,               // Data value.
	DWORD dwTimerLowValue,      // Timer low value.
	DWORD dwTimerHighValue ) {  // Timer high value.

	CPeriodicMsg * me = (CPeriodicMsg*)lpArg;
	me->TimerSignaled();
}

void CPeriodicMsg::TimerSignaled()
{
	LOG(HELPERFUNC,"CPeriodicMsg::TimerSignaled: Id 0x%x - waiting for semaphore",id);
	lock.Lock();
	int ret;
//	LOG(HELPERFUNC,"CPeriodicMsg::TimerSignaled: Id %d - sending msg",id);
	if ((ret=callback->SendPeriodicMsg(msg,id)) != STATUS_NOERROR)
	{
		LOG(HELPERFUNC,"CPeriodicMsg::TimerSignaled: callback return error %d !",ret);
	}
	else
	{
		LOG(HELPERFUNC,"CPeriodicMsg::TimerSignaled: periodic msg sent succesfully!");
	}
	lock.Unlock();
}

bool CPeriodicMsg::HasTimer()
{
	return !(hTimer==NULL);
}

int CPeriodicMsg::AttachMessage(PASSTHRU_MSG * pMsg)
{
	msg = CloneMsg(pMsg);

	if (msg==NULL)
		return ERROR_OUTOFMEMORY;
	return STATUS_NOERROR;
}

// This function copied mostly from MSDN snippet
int CPeriodicMsg::CreateTimer()
{
	BOOL            bSuccess;
	__int64         qwDueTime;
	LARGE_INTEGER   liDueTime;

	LOG(HELPERFUNC,"CPeriodicMsg::CreateTimer: id 0x%x, time interval %d milliseconds",id,timeInterval);

	if (!msg)
	{
		LOG(ERR,"CPeriodicMsg::CreateTimer: message not yet attached!");
		return ERR_FAILED;
	}

//	char timerName[32];
//	sprintf_s(timerName,32,"PeriodicMsg%x",id);

	if ( hTimer = CreateWaitableTimerA( NULL, FALSE, NULL) ) { // timerName ) ) {      

		LOG(HELPERFUNC,"CPeriodicMsg::CreateTimer: created timer"); // with name [%s]",timerName);

		// Create a negative 64-bit integer that will be used to
		// signal the timer first time FIRST_TIME_SIGNALING milliseconds from now.
		qwDueTime = -FIRST_TIME_SIGNALING * _MILLISECOND;

		// Copy the relative time into a LARGE_INTEGER.
		liDueTime.LowPart  = (DWORD) ( qwDueTime & 0xFFFFFFFF );
		liDueTime.HighPart = (LONG)  ( qwDueTime >> 32 );

		bSuccess = SetWaitableTimer(
			hTimer,                 // Handle to the timer object.
			&liDueTime,             // When timer will become signaled first time
			timeInterval,           // Periodic timer interval of (TimeInterval) milliseconds
			Timer,           // Completion routine.
			(void*)this,                // Argument to the completion routine.
			FALSE );                // Do not restore a suspended system.

		if ( bSuccess ) {
			return STATUS_NOERROR;
		} else {
			int err = GetLastError();
			LOG(ERR,"CPeriodicMsg::CreateTimer: failed setting timer with error %d",err);
			return err;
		}			

	} else {
		int err = GetLastError();
		LOG(ERR,"CPeriodicMsg::CreateTimer: failed creating timer with error %d",err);
		return err;
	}

}



CPeriodicMsg::CPeriodicMsg(CPeriodicMsgCallback * Callback, unsigned long Id, unsigned long TimeInterval)
{
	callback = Callback;
	id = Id;
	timeInterval = TimeInterval;
	msg=NULL;
	hTimer=NULL;
}


CPeriodicMsg::~CPeriodicMsg(void)
{

	if (msg)
		delete msg;
	if (hTimer)
	{
	 CancelWaitableTimer(hTimer);
	}
	CloseHandle( hTimer );
}
