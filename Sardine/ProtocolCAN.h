#pragma once
#include "protocol.h"

#define CAN_MSG_LEVEL_DEBUGGING

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
	*/
	virtual int IOCTL(unsigned long IoctlID, void *pInput, void *pOutput);


	virtual int DoWriteMsg( PASSTHRU_MSG * pMsg, unsigned long Timeout );
	virtual bool HandleMsg( PASSTHRU_MSG * pMsg, char * flags, int flagslen );

protected:
	virtual int GetIOCTLParam( SCONFIG * pConfig );
	virtual int SetIOCTLParam( SCONFIG * pConfig );

private:



	int extended_addressing;
	addr_len address_len;
};

