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

