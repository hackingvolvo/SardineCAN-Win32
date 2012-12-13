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
#include "ProtocolCAN.h"
#include "sardine_defs.h"
#include "helper.h"
#include "assert.h"


bool CProtocolCAN::HandleMsg( PASSTHRU_MSG * pMsg, char * flags)
{
	LOG(PROTOCOL_MSG,"CProtocolCAN::HandleMsg: flags [%s]",flags);
	int flagslen=strlen(flags);
	if (flagslen<4)
		{
		LOG(ERR,"CProtocolCAN::HandleMsg: More flags required! flagslen %d",flagslen);
		return false;
		}
	char protocol_type = flags[0];
	char message_type = flags[1];
	char addr_type = flags[2];
	int payload_len = convert_ascii_char_to_nibble(flags[3]);

	LOG(PROTOCOL_MSG,"CProtocolCAN::HandleMsg: protocol_type %c, msg_type %c, addr_type %c, payload_len %d",protocol_type,message_type,addr_type,payload_len);

	if (protocol_type=='c') // CAN bus message
		{
		// new CAN bus message
		pMsg->ProtocolID = ProtocolID(); // CAN; 
		if (pMsg->DataSize != payload_len + 4)
			{
			LOG(ERR,"CProtocolCAN::HandleMsg: Mismatch in msg size (DataSize %d != specified payload length %d + 4)",pMsg->DataSize,payload_len);
			return false;
			}
		if (message_type=='n')
			{
			LOG(PROTOCOL_MSG_VERBOSE,"CProtocolCAN::HandleMsg: normal message");
			}
		else if (message_type=='r')
			{
			LOG(PROTOCOL_MSG,"CProtocolCAN::HandleMsg: RTR message");
			}
		else
			{
			LOG(ERR,"CProtocolCAN::HandleMsg: abnormal message type! [%c] ",protocol_type);
			return false;
			}

		if (addr_type=='x')
			{
			LOG(PROTOCOL_MSG_VERBOSE,"CProtocolCAN::HandleMsg: extended addressing (29 bit)");
			pMsg->RxStatus |= CAN_29BIT_ID;
			}
		else if (addr_type=='b')
			{
			LOG(PROTOCOL_MSG_VERBOSE,"CProtocolCAN::HandleMsg: standard addressing (11 bit)");
			pMsg->RxStatus &= ~CAN_29BIT_ID;
			}
		else
			{
			LOG(ERR,"CProtocolCAN::HandleMsg: abnormal addressing type! [%c] ",addr_type);
			return false;
			}

		pMsg->ExtraDataIndex = pMsg->DataSize;	// we don't have any checksum
		return true;
		}
	else
		{
		LOG(PROTOCOL_MSG,"CProtocolCAN::HandleMsg: was not a CAN message");
		}
	return false;
}


CProtocolCAN::CProtocolCAN(int ProtocolID)
	:CProtocol(ProtocolID)
{
}


CProtocolCAN::~CProtocolCAN(void)
{
}


int CProtocolCAN::Connect(unsigned long channelId, unsigned long Flags)
{
	LOG(PROTOCOL,"CProtocolCAN::Connect - flags: 0x%x",Flags);
	this->address_len = (Flags & CAN_29BIT_ID) ? CAN29bit : CAN11bit;
	this->extended_addressing = (Flags>>7) & 1;
	LOG(PROTOCOL,"CProtocolCAN::Connect - CAN29bit: %d, ext_addressing: %d",address_len,extended_addressing);

	// call base class implementation for general settings
	return CProtocol::Connect(channelId,Flags);
}


int CProtocolCAN::Disconnect() 
{
	LOG(PROTOCOL,"CProtocolCAN::Disconnect");
	return CProtocol::Disconnect();
}


int CProtocolCAN::WriteMsg( PASSTHRU_MSG * pMsg, unsigned long Timeout )
{
	LOG(PROTOCOL,"CProtocolCAN::DoWriteMsg - timeout %d",Timeout);
	LOG(ERR,"CProtocolCAN::DoWriteMsg  --- FIXME -- We ignore Timeout for now - call will be blocking");

	if ( (pMsg->DataSize <4) || (pMsg->DataSize>12) )
	{
		LOG(ERR,"CProtocolCAN::DoWriteMsg - invalid data length: %d",pMsg->DataSize);
		return ERR_INVALID_MSG;
	}

	char flags[5];
	flags[0] = 'c'; // protocol type
	flags[1] = 'n'; // message type: normal message (RTR to be done) (FIXME)
	flags[2] = (pMsg->TxFlags & CAN_29BIT_ID) ? 'x' : 'b';  // x==extended addressing (29-bit), b=normal 11-bit addressing.
	flags[3] = '0' + (char)(pMsg->DataSize - 4); // payload length (data size - id length) in hexadecimal.
	flags[4] = 0;

	return CProtocol::DoWriteMsg(pMsg,flags,Timeout);
}



int CProtocolCAN::GetIOCTLParam( SCONFIG * pConfig )
{
	LOGW(PROTOCOL,_T("CProtocolCAN::GetIOCTLParam - parameter %d [%s]"), pConfig->Parameter,dbug_param2str(pConfig->Parameter));
	switch(pConfig->Parameter)
		{
		case BIT_SAMPLE_POINT:
			LOG(ERR,"CProtocolCAN::GetIOCTLParam - Querying BIT_SAMPLE_POINT from device not implemented, returning default: 80 --- FIXME");
			pConfig->Value = 80;
			return STATUS_NOERROR;
		case SYNC_JUMP_WIDTH:
			LOG(ERR,"CProtocolCAN::GetIOCTLParam - Querying SYNC_JUMP_WIDTH not implemented, returning default: 15 --- FIXME");
			pConfig->Value = 15;
			return STATUS_NOERROR;
		default:
			LOG(PROTOCOL,"CProtocolCAN::GetIOCTLParam - reverting to base class handler");
			return CProtocol::GetIOCTLParam(pConfig);
		}

	// never reach this
	assert(0);
	return ERR_FAILED;
}


int CProtocolCAN::SetIOCTLParam( SCONFIG * pConfig )
{
	LOGW(PROTOCOL,_T("CProtocolCAN::SetIOCTLParam - parameter %d [%s]"), pConfig->Parameter,dbug_param2str(pConfig->Parameter));
	switch(pConfig->Parameter)
		{
		case BIT_SAMPLE_POINT:
#ifdef IGNORE_SILENTLY_UNIMPLEMENTED_FEATURES
			LOG(ERR,"CProtocolCAN::SetIOCTLParam - ignoring set BIT_SAMPLE_POINT --- FIXME");
			return STATUS_NOERROR;
#else
			LOG(ERR,"CProtocolCAN::SetIOCTLParam - unsupported parameter! --- FIXME");
			return ERR_NOT_SUPPORTED;
#endif
		case SYNC_JUMP_WIDTH:
#ifdef IGNORE_SILENTLY_UNIMPLEMENTED_FEATURES
			LOG(ERR,"CProtocolCAN::SetIOCTLParam - ignoring set SYNC_JUMP_WIDTH --- FIXME");
			return STATUS_NOERROR;
#else
			LOG(ERR,"CProtocolCAN::SetIOCTLParam - unsupported parameter! --- FIXME");
			return ERR_NOT_SUPPORTED;
#endif
		default:
			LOG(PROTOCOL,"CProtocolCAN::SetIOCTLParam - reverting to base class handler");
			return CProtocol::SetIOCTLParam(pConfig);
		}

	// never reach this
	assert(0);
	return ERR_FAILED;
}

/*
int CProtocolCAN::IOCTL(unsigned long IoctlID, void *pInput, void *pOutput)
{
	LOG(PROTOCOL,"CProtocolCAN::IOCTL - ioctl command %d", IoctlID);
//	unsigned int err;

	switch (IoctlID)
	{
	default:
		LOG(MAINFUNC,"CProtocolCAN::IOCTL reverting to base handler");
		return CProtocol::IOCTL(IoctlID,pInput,pOutput);
		break;

	}

	// never reach this
	assert(0);
	return ERR_FAILED;
}
*/
