#pragma once
#include "sardine_defs.h"

class CInterceptorCallback
{
public:
	virtual int SendInterceptorMessage( PASSTHRU_MSG * pMsg ) = 0;	// ownership is given
};

class CInterceptor
{
public:
	bool SendingMsg( PASSTHRU_MSG * pMsg );
	bool ReceivingMsg( PASSTHRU_MSG * pMsg );
	CInterceptor(CInterceptorCallback * Callback, int protocolId);
	~CInterceptor(void);

	static bool UseInterceptor();
private:
	bool SendRelatedMsgs(  PASSTHRU_MSG * pMsg, int i );
	CInterceptorCallback * callback;
};

