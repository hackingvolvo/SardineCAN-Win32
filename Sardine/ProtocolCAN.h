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

class CProtocolCAN :
	public CProtocol
{
public:
	CProtocolCAN( int ProtocolID );
	~CProtocolCAN(void);

	enum addr_len {
		CAN29bit,
		CAN11bit 
	};

	// Passthru Handlers
	virtual int Connect(unsigned long channelId, unsigned long Flags);
	virtual int Disconnect() ;
	/*
	virtual int ReadMsgs( PASSTHRU_MSG * pMsgs, unsigned long * pNumMsgs, unsigned long Timeout );
	virtual int StartPeriodicMsg( PASSTHRU_MSG * pMsg , unsigned long * pMsgID, unsigned long TimeInterval);
	virtual int StopPeriodicMsg( unsigned long MsgID);
	virtual int IOCTL(unsigned long IoctlID, void *pInput, void *pOutput);
	*/


	virtual int WriteMsg( PASSTHRU_MSG * pMsg, unsigned long Timeout );
	virtual bool HandleMsg( PASSTHRU_MSG * pMsg, char * flags);

protected:
	virtual int GetIOCTLParam( SCONFIG * pConfig );
	virtual int SetIOCTLParam( SCONFIG * pConfig );

private:



	int extended_addressing;
	addr_len address_len;
};

