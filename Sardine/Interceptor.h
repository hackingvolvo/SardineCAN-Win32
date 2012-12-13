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

class CInterceptorCallback
{
public:
	virtual int SendInterceptorMessage( PASSTHRU_MSG * pMsg ) = 0;	// ownership is given
};


// class to intercept the sent messages, check if they match a preprogrammed filter and if match is found, then block the message and send message(s) back to the receive buffer
// This is to get VIDA up and running without having to connect it to a car when testing. (For example a query for VIN will be blocked and our stored reply will be sent back to VIDA instead)

class CInterceptor
{
public:
	// intercept sent messages
	bool DoesMatchInterceptorFilter( PASSTHRU_MSG * pMsg );

	CInterceptor(CInterceptorCallback * Callback, int protocolId);
	~CInterceptor(void);

	// check if interceptor is enabled in Windows Registry
	static bool UseInterceptor();
private:
	bool SendRelatedMsgs(  PASSTHRU_MSG * pMsg, int i );
	CInterceptorCallback * callback;
};

