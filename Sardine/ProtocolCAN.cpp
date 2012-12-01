#include "StdAfx.h"
#include "ProtocolCAN.h"
#include "sardine_defs.h"
#include "helper.h"
#include "assert.h"




bool CProtocolCAN::HandleMsg( PASSTHRU_MSG * pMsg, char * flags, int flagslen)
{
	LOG(PROTOCOL_MSG,"CProtocolCAN::HandleMsg: flags [%s]",flags);
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
		pMsg->ProtocolID = ProtocolID(); // CAN; //PROTOCOL_ID_RAW_CAN;
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
	this->address_len = ((Flags>>8)&1) ? CAN29bit : CAN11bit;
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





int CProtocolCAN::DoWriteMsg( PASSTHRU_MSG * pMsg, unsigned long Timeout )
{
	LOG(PROTOCOL,"CProtocolCAN::DoWriteMsg - timeout %d",Timeout);
	LOG(ERR,"CProtocolCAN::DoWriteMsg  --- FIXME -- We ignore Timeout for now - call will be blocking");

	if ( (pMsg->DataSize <4) || (pMsg->DataSize>12) )
	{
		LOG(ERR,"CProtocolCAN::DoWriteMsg - invalid data length: %d",pMsg->DataSize);
		return ERR_INVALID_MSG;
	}

	return CProtocol::DoWriteMsg(pMsg,Timeout);
}

/*
int CProtocolCAN::StartPeriodicMsg( PASSTHRU_MSG * pMsg, unsigned long * pMsgID, unsigned long TimeInterval)
{
	LOG(PROTOCOL,"CProtocolCAN::StartPeriodicMsg - time interval %d milliseconds", TimeInterval);
	dbug_printmsg(pMsg,_T("PeriodicMsg"),1,true);

#ifdef PLAY_STUPID
	LOG(ERR,"CProtocolCAN::StartPeriodicMsg - not yet implemented, ignored");
	*pMsgID=_dummy_periodic_msg_id++;
	return STATUS_NOERROR;
#else
	LOG(ERR,"CProtocolCAN::StartPeriodicMsg  --- FIXME ! does not exist");
	return ERR_NOT_SUPPORTED;
#endif
}

int CProtocolCAN::StopPeriodicMsg( unsigned long MsgID)
{
	LOG(PROTOCOL,"CProtocolCAN::StopPeriodicMsg - msg id %d",MsgID);
#ifdef PLAY_STUPID
	LOG(ERR,"CProtocolCAN::StopPeriodicMessages - not yet implemented, ignored");
	return STATUS_NOERROR;
#else
	LOG(ERR,"CProtocolCAN::StopPeriodicMessages  --- FIXME ! does not exist");
	return ERR_NOT_SUPPORTED;
#endif
}
*/


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
	return STATUS_NOERROR;
}


int CProtocolCAN::SetIOCTLParam( SCONFIG * pConfig )
{
	LOGW(PROTOCOL,_T("CProtocolCAN::SetIOCTLParam - parameter %d [%s]"), pConfig->Parameter,dbug_param2str(pConfig->Parameter));
	switch(pConfig->Parameter)
		{
		case BIT_SAMPLE_POINT:
#ifdef PLAY_STUPID
			LOG(ERR,"CProtocolCAN::SetIOCTLParam - ignoring set BIT_SAMPLE_POINT --- FIXME");
			return STATUS_NOERROR;
#else
			LOG(ERR,"CProtocolCAN::SetIOCTLParam - unsupported parameter! --- FIXME");
			return ERR_NOT_SUPPORTED;
#endif
		case SYNC_JUMP_WIDTH:
#ifdef PLAY_STUPID
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
	return STATUS_NOERROR;
}

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
	return STATUS_NOERROR;
}
