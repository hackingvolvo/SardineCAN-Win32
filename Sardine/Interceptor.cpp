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
#include "Interceptor.h"
#include "helper.h"
#include "registry.h"

#include "../private/my_volvo_s80_2002.h"	// comment this and uncomment the line below (see Interceptor section in MANUAL.txt for details)
//#include "my_volvo_stub.h"


#define FILTERS 1
#define PATTERNS_ITEMS 3
#define PATTERN_ITEM_SIZE 17

CInterceptor::CInterceptor(CInterceptorCallback * Callback, int protocolId)
{
	callback = Callback;
}


CInterceptor::~CInterceptor(void)
{
}

// check if interceptor is enabled in Windows Registry
bool CInterceptor::UseInterceptor()
{
	unsigned long useInterceptor=0;
	SardineRegistry::GetValueFromRegistry(NULL,L"UseInterceptor",&useInterceptor);
	LOG(HELPERFUNC,"CInterceptor::UseInterceptor %d",useInterceptor);
	return useInterceptor==1;
}


bool CInterceptor::SendRelatedMsgs(  PASSTHRU_MSG * pMsg, int msg_index )
{
	int msgCount=filters[msg_index].msgCount;
	LOG(MAINFUNC,"CInterceptor::SendRelatedMsgs: matches msg #d%d in db. Sending %d related msgs",msg_index+1,msgCount);
	for (int i=0;i<msgCount;i++)
	{
		int currMsg = filters[msg_index].msgs[i];
		PASSTHRU_MSG * msg = new PASSTHRU_MSG;
		memset(msg,0,sizeof(PASSTHRU_MSG));
		msg->ProtocolID = pMsg->ProtocolID;
		msg->RxStatus = pMsg->TxFlags & CAN_29BIT_ID;
		msg->DataSize = msgs[currMsg].size;
		for (unsigned int j=0;j<msg->DataSize;j++)
			{
				msg->Data[j] = msgs[currMsg].data[j];
			}

		LOG(MAINFUNC,"CInterceptor::SendRelatedMsgs: sending msg #%d",i+1);
		callback->SendInterceptorMessage(msg);	// ownership is given
	}
	return true;
}

bool CInterceptor::DoesMatchInterceptorFilter( PASSTHRU_MSG * pMsg )
{
	LOG(MAINFUNC,"CInterceptor::SendingMsg: protocol Id 0x%x, size: %d, TxFlags 0x%x",pMsg->ProtocolID,pMsg->DataSize,pMsg->TxFlags);
	if ( (pMsg->ProtocolID==CAN) || (pMsg->ProtocolID==CAN_PS) )
	{
		for (int i=0;i<FILTERS;i++)
		{
			if (filters[i].size == pMsg->DataSize)
			{
				int match=0;
				for (unsigned int j=0;j<filters[i].size;j++)
					if ( (pMsg->Data[j] & filters[i].mask[j]) == filters[i].pattern[j])
						match++;
				if (match==pMsg->DataSize)
					{
					SendRelatedMsgs(pMsg,i);
					return true;
					}
			}
		}
	}
	return false;
}
