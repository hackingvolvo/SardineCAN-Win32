#include "stdafx.h"
#include "Interceptor.h"
#include "helper.h"
#include "registry.h"

static const struct interceptorMsg {
	unsigned int size;
	unsigned char data[12];
} msgs[] =
{
	//const unsigned char msg_db[13][13] = {
{12,{ 0x00, 0x80, 0x00, 0x03, 0x8F, 0x40, 0xF9, 0xFB, 0x57, 0x00, 0x31, 0x80 }},
{12,{ 0x00, 0x80, 0x00, 0x03, 0x09, 0x36, 0x45, 0x30, 0x30, 0x34, 0x0D, 0x0A }},
{12,{ 0x00, 0x80, 0x00, 0x03, 0x0A, 0x59, 0x56, 0x31, 0x54, 0x53, 0x37, 0x39 }},
{12,{ 0x00, 0x80, 0x00, 0x03, 0x0B, 0x33, 0x39, 0x32, 0x31, 0x32, 0x38, 0x31 }},
{12,{ 0x00, 0x80, 0x00, 0x03, 0x0C, 0x34, 0x39, 0x39, 0x0D, 0x0A, 0x31, 0x38 }},
{12,{ 0x00, 0x80, 0x00, 0x03, 0x0D, 0x36, 0x39, 0x39, 0x33, 0x32, 0x33, 0x34 }},
{12,{ 0x00, 0x80, 0x00, 0x03, 0x0E, 0x0D, 0x0A, 0x31, 0x38, 0x34, 0x0D, 0x0A }},
{12,{ 0x00, 0x80, 0x00, 0x03, 0x0F, 0x32, 0x38, 0x31, 0x34, 0x39, 0x39, 0x0D }},
{12,{ 0x00, 0x80, 0x00, 0x03, 0x08, 0x0A, 0x32, 0x31, 0x0D, 0x0A, 0x32, 0x30 }},
{12,{ 0x00, 0x80, 0x00, 0x03, 0x09, 0x30, 0x32, 0x32, 0x37, 0x0D, 0x0A, 0x30 }},
{12,{ 0x00, 0x80, 0x00, 0x03, 0x0A, 0x30, 0x30, 0x30, 0x30, 0x34, 0x31, 0x32 }},
{12,{ 0x00, 0x80, 0x00, 0x03, 0x0B, 0x39, 0x33, 0x31, 0x34, 0x30, 0x38, 0x32 }},
{12,{ 0x00, 0x80, 0x00, 0x03, 0x4F, 0x37, 0x35, 0x37, 0x37, 0x35, 0x0D, 0x0A} } 
};

#define FILTERS 1
#define PATTERNS_ITEMS 3
#define PATTERN_ITEM_SIZE 17

static const struct interceptorFilter {
	unsigned int size;
	unsigned char pattern[12];
	unsigned char mask[12];
	unsigned int msgCount;	// how many messages to send
	unsigned int msgs[16];	// msg index numbers referring to msg_db. Now maximum of 16 messages can be sent as a reaction to a filter match
} filters[] =
{
	{ 
		12,	{ 0x00,0x0f,0xff,0xfe,0xcb,0x40,0xb9,0xfb,0x00,0x00,0x00,0x00},
			{ 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00},
		13, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0,0,0 }
	}
};

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
