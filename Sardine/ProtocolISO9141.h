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
	int ReadMsgs( PASSTHRU_MSG * pMsg, unsigned long * pNumMsgs, unsigned long Timeout );
	int DoWriteMsg( PASSTHRU_MSG * pMsg, unsigned long Timeout );
	bool HandleMsg( PASSTHRU_MSG * pMsg, char * flags, int flagslen );
	int IOCTL(unsigned long IoctlID, void *pInput, void *pOutput);

private:
	int GetIOCTLParam( SCONFIG * pConfig );
	int SetIOCTLParam( SCONFIG * pConfig );
};

