#include "stdafx.h"
#include "ProtocolJ1850VPW.h"
#include "helper.h"


CProtocolJ1850VPW::CProtocolJ1850VPW(int ProtocolID)
	:CProtocol(ProtocolID)
{
}


CProtocolJ1850VPW::~CProtocolJ1850VPW(void)
{
}

bool CProtocolJ1850VPW:: HandleMsg( PASSTHRU_MSG * pMsg, char * flags, int flagslen )
{
	return false;
}

int CProtocolJ1850VPW::Connect(unsigned long channelId, unsigned long Flags)
{
#ifdef PLAY_STUPID_WITH_DATAPRO
	LOG(PROTOCOL,"CProtocolJ1850VPW::Connect - flags: 0x%x",Flags);

	// call base class implementation for general settings
	return CProtocol::Connect(channelId,Flags);
#else
	return ERR_NOT_SUPPORTED;
#endif
}

int CProtocolJ1850VPW::Disconnect() 
{
#ifdef PLAY_STUPID_WITH_DATAPRO
	return CProtocol::Disconnect();
#else
	return ERR_NOT_SUPPORTED;
#endif
}

int CProtocolJ1850VPW::ReadMsgs( PASSTHRU_MSG * pMsgs, unsigned long * pNumMsgs, unsigned long Timeout )
{
	return ERR_NOT_SUPPORTED;
}

int CProtocolJ1850VPW::DoWriteMsg( PASSTHRU_MSG * pMsg, unsigned long Timeout )
{
#ifdef PLAY_STUPID_WITH_DATAPRO
	LOG(PROTOCOL_MSG,"CProtocolJ1850VPW::DoWriteMsg - timeout %d",Timeout);
	LOG(ERR,"CProtocolJ1850VPW::DoWriteMsg  --- FIXME -- We ignore Timeout for now - call will be blocking");

	if (pMsg->ProtocolID != CAN) //PROTOCOL_ID_RAW_CAN)
	{
		LOG(ERR,"CProtocolJ1850VPW::DoWriteMsg - invalid protocol id %d != RAW_CAN",pMsg->ProtocolID);
		return ERR_MSG_PROTOCOL_ID;
	}
	if ( (pMsg->DataSize <4) || (pMsg->DataSize>12) )
	{
		LOG(ERR,"CProtocolJ1850VPW::DoWriteMsg - invalid data length: %d",pMsg->DataSize);
		return ERR_INVALID_MSG;
	}

	char buf[256];		
	unsigned int j=0;
	for (j=0;j<pMsg->DataSize;j++)
		sprintf_s(&buf[j*3],256-j*3,"%02x ",pMsg->Data[j]);
	int msg_len = j*3 -1;
	buf[msg_len]=0;

	LOG(PROTOCOL_MSG_VERBOSE,"CProtocolJ1850VPW::DoWriteMsg - pretending to send msg: [%s]",buf);
	/*
	// Delegate message sending to lower level
	if (SendMsg(buf,msg_len)!=(msg_len))
	{
		LOG(ERR,"CProtocolJ1850VPW::DoWriteMsg - sending message failed!");
		return ERR_FAILED;
	}
	*/
	return STATUS_NOERROR;
#else
	return ERR_NOT_SUPPORTED;
#endif
}

/*
int CProtocolJ1850VPW::StartPeriodicMsg( PASSTHRU_MSG * pMsg, unsigned long * pMsgID, unsigned long TimeInterval)
{
	return ERR_NOT_SUPPORTED;
}

int CProtocolJ1850VPW::StopPeriodicMsg( unsigned long pMsgID)
{
	return ERR_NOT_SUPPORTED;
}

int CProtocolJ1850VPW::IOCTL(unsigned long IoctlID, void *pInput, void *pOutput)
{
#ifdef PLAY_STUPID_WITH_DATAPRO
	return STATUS_NOERROR;
#else
	return ERR_NOT_SUPPORTED;
#endif
}

*/
