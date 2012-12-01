#pragma once
#include "sardine_defs.h"
#include "Benaphore.h"

class CPeriodicMsgCallback
{
public:
	virtual int SendPeriodicMsg( PASSTHRU_MSG * pMsg, unsigned long Id ) = 0;
};

class CPeriodicMsg
{
public:
	CPeriodicMsg( CPeriodicMsgCallback * Callback, unsigned long Id, unsigned long TimeInterval);
	int AttachMessage( PASSTHRU_MSG * pMsg );
	int CreateTimer();
	bool HasTimer();
	void TimerSignaled();
	~CPeriodicMsg(void);
	const unsigned long Id() { return id; }

	HANDLE Handle();
private:
	CPeriodicMsgCallback * callback;
	HANDLE hTimer;
	PASSTHRU_MSG * msg;
	unsigned long id;
	unsigned long timeInterval;
	Benaphore lock;
};

