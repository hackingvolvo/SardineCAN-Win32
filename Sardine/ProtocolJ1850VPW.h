#pragma once
#include "protocol.h"
class CProtocolJ1850VPW :
	public CProtocol
{
public:
	CProtocolJ1850VPW(int ProtocolID);
	~CProtocolJ1850VPW(void);

	int Connect(unsigned long channelId, unsigned long Flags);
	int Disconnect() ;
	int ReadMsgs( PASSTHRU_MSG * pMsg, unsigned long * pNumMsgs, unsigned long Timeout );
	int DoWriteMsg( PASSTHRU_MSG * pMsg, unsigned long Timeout );

	bool HandleMsg( PASSTHRU_MSG * pMsg, char * flags, int flagslen );
//	int IOCTL(unsigned long IoctlID, void *pInput, void *pOutput);

};

