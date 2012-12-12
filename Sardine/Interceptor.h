#pragma once
#include "sardine_defs.h"

class CInterceptorCallback
{
public:
	virtual int SendInterceptorMessage( PASSTHRU_MSG * pMsg ) = 0;	// ownership is given
};


// class to intercept the sent messages, check if they match a preprogrammed filter and if match is found, then block the message and send message(s) back to the receive buffer
// This is to get VIDA up and running without it connected to a car when testing. (Query for VIN will be blocked and our stored reply will be sent to VIDA instead)

class CInterceptor
{
public:
	// intercept sent messages
	bool DoesMatchInterceptorFilter( PASSTHRU_MSG * pMsg );

	CInterceptor(CInterceptorCallback * Callback, int protocolId);
	~CInterceptor(void);

	// check if interceptor is enabled in Windows Registry
	static bool UseInterceptor();
private:
	bool SendRelatedMsgs(  PASSTHRU_MSG * pMsg, int i );
	CInterceptorCallback * callback;
};

