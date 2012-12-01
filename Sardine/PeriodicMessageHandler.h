#pragma once
#include "PeriodicMsg.h"
#include "Benaphore.h"

#define MAX_PERIODIC_MSGS 10

class CPeriodicMessageHandler
{
public:
	CPeriodicMessageHandler(void);
	~CPeriodicMessageHandler(void);
	int AddPeriodicMessage(CPeriodicMsg * msg);
	int RemovePeriodicMessage(unsigned long Id);
	bool createMsgHandlerThread(int channelId);
	void RemoveAllPeriodicMessages();

	bool HandleSignal(void);


private:
	CPeriodicMsg * periodicMessages[MAX_PERIODIC_MSGS];
	int pMsgNum;
	HANDLE hMsgHandlerThread;
	HANDLE hMsgHandlerEvent;

	int stopped;
	Benaphore HandlerLock;
};

