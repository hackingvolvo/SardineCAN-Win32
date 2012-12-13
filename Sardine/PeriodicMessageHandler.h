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

