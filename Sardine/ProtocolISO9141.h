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
#include "protocol.h"
class CProtocolISO9141 :
	public CProtocol
{
public:
	CProtocolISO9141(int ProtocolID);
	~CProtocolISO9141(void);
	
	int Connect(unsigned long channelId, unsigned long Flags);
	int Disconnect() ;
//	int ReadMsgs( PASSTHRU_MSG * pMsg, unsigned long * pNumMsgs, unsigned long Timeout );
	int WriteMsg( PASSTHRU_MSG * pMsg, unsigned long Timeout );
	bool HandleMsg( PASSTHRU_MSG * pMsg, char * flags );
	int IOCTL(unsigned long IoctlID, void *pInput, void *pOutput);

private:
	int GetIOCTLParam( SCONFIG * pConfig );
	int SetIOCTLParam( SCONFIG * pConfig );
};

