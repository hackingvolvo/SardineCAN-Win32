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

#include "StdAfx.h"
#include "ProtocolISO9141.h"
#include "sardine_defs.h"
#include "helper.h"
#include <assert.h>

CProtocolISO9141::CProtocolISO9141(int ProtocolID )
	:CProtocol(ProtocolID)
{
}


CProtocolISO9141::~CProtocolISO9141(void)
{
}

bool CProtocolISO9141:: HandleMsg( PASSTHRU_MSG * pMsg, char * flags )
{
	LOG(PROTOCOL_MSG,"CProtocolISO9141::HandleMsg - ignoring msg");
	return false;
}

int CProtocolISO9141::Connect(unsigned long channelId, unsigned long Flags)
{
	LOG(PROTOCOL,"CProtocolISO9141::Connect - flags: 0x%x",Flags);
#ifdef IGNORE_SILENTLY_UNIMPLEMENTED_FEATURES
	LOG(ERR,"CProtocolISO9141::Connect - not yet implemented, ignoring");
	// call base class implementation for general settings
	return CProtocol::Connect(channelId,Flags);
#else
	LOG(ERR,"CProtocolISO9141::Connect - not yet implemented!");
	return ERR_NOT_SUPPORTED;
#endif
}

int CProtocolISO9141::Disconnect() 
{
	LOG(PROTOCOL,"CProtocolISO9141::Disconnect");
#ifdef IGNORE_SILENTLY_UNIMPLEMENTED_FEATURES
	LOG(ERR,"CProtocolISO9141::Disconnect - not yet implemented, ignoring");
	return CProtocol::Disconnect();
#else
	LOG(ERR,"CProtocolISO9141::Disconnect - not yet implemented!");
	return ERR_NOT_SUPPORTED;
#endif
}

/*
int CProtocolISO9141::ReadMsgs( PASSTHRU_MSG * pMsgs, unsigned long * pNumMsgs, unsigned long Timeout )
{
	LOG(PROTOCOL,"CProtocolISO9141::ReadMsgs - pNumMsgs %d, timeout %d",*pNumMsgs,Timeout);
#ifdef IGNORE_SILENTLY_UNIMPLEMENTED_FEATURES
	LOG(ERR,"CProtocolISO9141::ReadMsgs - not yet implemented, ignoring");
	return STATUS_NOERROR;
#else
	LOG(ERR,"CProtocolISO9141::ReadMsgs - not yet implemented!");
	return ERR_NOT_SUPPORTED;
#endif
}
*/

int CProtocolISO9141::WriteMsg( PASSTHRU_MSG * pMsg, unsigned long Timeout )
{
	LOG(PROTOCOL,"CProtocolISO9141::DoWriteMsg - timeout %d ",Timeout);
#ifdef IGNORE_SILENTLY_UNIMPLEMENTED_FEATURES
	LOG(ERR,"CProtocolISO9141::DoWriteMsg - not yet implemented, ignoring");
	return STATUS_NOERROR;
#else
	LOG(ERR,"CProtocolISO9141::DoWriteMsg - not yet implemented!");
	return ERR_NOT_SUPPORTED;
#endif
}

/*
int CProtocolISO9141::StartPeriodicMsg( PASSTHRU_MSG * pMsgs, unsigned long * pMsgID, unsigned long TimeInterval)
{
	LOG(PROTOCOL,"CProtocolISO9141::StartPeriodicMsg - time interval %d, ",TimeInterval);
#ifdef IGNORE_SILENTLY_UNIMPLEMENTED_FEATURES
	LOG(ERR,"CProtocolISO9141::StartPeriodicMsg - not yet implemented, ignoring");
	return STATUS_NOERROR;
#else
	LOG(ERR,"CProtocolISO9141::StartPeriodicMsg - not yet implemented!");
	return ERR_NOT_SUPPORTED;
#endif
}

int CProtocolISO9141::StopPeriodicMsg( unsigned long pMsgID)
{
	LOG(PROTOCOL,"CProtocolISO9141::StopPeriodicMsg ");
#ifdef IGNORE_SILENTLY_UNIMPLEMENTED_FEATURES
	LOG(ERR,"CProtocolISO9141::StopPeriodicMsg - not yet implemented, ignoring");
	return STATUS_NOERROR;
#else
	LOG(ERR,"CProtocolISO9141::StopPeriodicMsg - not yet implemented!");
	return ERR_NOT_SUPPORTED;
#endif
}
*/

int CProtocolISO9141::GetIOCTLParam( SCONFIG * pConfig )
{
	LOGW(PROTOCOL,_T("CProtocolISO9141::GetIOCTLParam - parameter %d [%s]"), pConfig->Parameter,dbug_param2str(pConfig->Parameter));
	switch(pConfig->Parameter)
		{
		case P1_MIN:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying P1_MIN from device not implemented, returning default: 0 ms --- FIXME");
			pConfig->Value = 0;
			return STATUS_NOERROR;
		case P1_MAX:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying P1_MAX from device not implemented, returning default: 20 ms --- FIXME");
			pConfig->Value = 20;
			return STATUS_NOERROR;
		case P2_MIN:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying P2_MIN from device not implemented, returning default: 25 ms --- FIXME");
			pConfig->Value = 25;
			return STATUS_NOERROR;
		case P2_MAX:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying P2_MAX from device not implemented, returning default: 50 ms --- FIXME");
			pConfig->Value = 50;
			return STATUS_NOERROR;
		case P3_MIN:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying P3_MIN from device not implemented, returning default: 55 ms --- FIXME");
			pConfig->Value = 55;
			return STATUS_NOERROR;
		case P3_MAX:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying P3_MAX from device not implemented, returning default: 5000 ms --- FIXME");
			pConfig->Value = 5000;
			return STATUS_NOERROR;
		case P4_MIN:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying P4_MIN from device not implemented, returning default: 5 ms --- FIXME");
			pConfig->Value = 5;
			return STATUS_NOERROR;
		case P4_MAX:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying P4_MAX from device not implemented, returning default: 20 ms --- FIXME");
			pConfig->Value = 20;
			return STATUS_NOERROR;
		case W0:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying W0 not implemented, returning default: 300 ms --- FIXME");
			pConfig->Value = 300;
			return STATUS_NOERROR;
		case W1:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying W1 not implemented, returning default: 300 ms --- FIXME");
			pConfig->Value = 300;
			return STATUS_NOERROR;
		case W2:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying W2 not implemented, returning default: 20 ms --- FIXME");
			pConfig->Value = 20;
			return STATUS_NOERROR;
		case W3:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying W3 not implemented, returning default: 20 ms --- FIXME");
			pConfig->Value = 20;
			return STATUS_NOERROR;
		case W4:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying W4 not implemented, returning default: 50 ms --- FIXME");
			pConfig->Value = 50;
			return STATUS_NOERROR;
		case W5:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying W5 not implemented, returning default: 300 ms --- FIXME");
			pConfig->Value = 300;
			return STATUS_NOERROR;
		case TIDLE:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying TIDLE not implemented, returning default: 300ms(==W5) --- FIXME");
			pConfig->Value = 300;
			return STATUS_NOERROR;
		case TINIL:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying TINL not implemented, returning default: 25 ms --- FIXME");
			pConfig->Value = 25;
			return STATUS_NOERROR;
		case TWUP:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying TWUP not implemented, returning default: 50 ms--- FIXME");
			pConfig->Value = 50;
			return STATUS_NOERROR;
		case PARITY:
			LOG(ERR,"CProtocolISO9141::GetIOCTLParam - Querying PARITY not implemented, returning default: 0 (no parity) --- FIXME");
			pConfig->Value = 0;
			return STATUS_NOERROR;
		default:
			LOG(PROTOCOL,"CProtocolISO9141::GetIOCTLParam - reverting to base class handler");
			return CProtocol::GetIOCTLParam(pConfig);
		}

	return STATUS_NOERROR;
}


int CProtocolISO9141::SetIOCTLParam( SCONFIG * pConfig )
{
	LOGW(PROTOCOL,_T("CProtocolISO9141::SetIOCTLParam - parameter %d [%s]"), pConfig->Parameter,dbug_param2str(pConfig->Parameter));
	switch(pConfig->Parameter)
		{
#ifdef IGNORE_SILENTLY_UNIMPLEMENTED_FEATURES
		case P1_MIN:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting P1_MIN on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case P1_MAX:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting P1_MAX on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case P2_MIN:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting P2_MIN on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case P2_MAX:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting P2_MAX on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case P3_MIN:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting P3_MIN on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case P3_MAX:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting P3_MAX on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case P4_MIN:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting P4_MIN on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case P4_MAX:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting P4_MAX on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case W0:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting W0 on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case W1:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting W1 on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case W2:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting W2 on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case W3:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting W3 on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case W4:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting W4 on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case W5:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting W5 on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case TIDLE:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting TIDLE on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case TINIL:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting TINL on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case TWUP:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting TWUP on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case PARITY:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting PARITY on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case FIVE_BAUD_MOD:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting FIVE_BAUD_MOD on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
		case DATA_BITS:
			LOG(ERR,"CProtocolISO9141::SetIOCTLParam - Setting DATA_BITS on device not implemented, ignoring --- FIXME");
			return STATUS_NOERROR;
#endif
		default:
			LOG(PROTOCOL,"CProtocolISO9141::SetIOCTLParam - reverting to base class handler");
			return CProtocol::SetIOCTLParam(pConfig);
		}

	return STATUS_NOERROR;
}

int CProtocolISO9141::IOCTL(unsigned long IoctlID, void *pInput, void *pOutput)
{
	LOG(PROTOCOL,"CProtocolISO9141::IOCTL - ioctl command %d", IoctlID);
//	unsigned int err;

	switch (IoctlID)
	{
	default:
		LOG(MAINFUNC,"CProtocolISO9141::IOCTL reverting to base handler");
		return CProtocol::IOCTL(IoctlID,pInput,pOutput);
		break;

	}

	return STATUS_NOERROR;
}