#include "StdAfx.h"
#include "Protocol.h"
#include <assert.h>
#include "helper.h"
#include "Arduino.h"
#include "shim_debug.h"
#include <string.h>

bool WINAPI ArduinoListener( char * msg, int len, void * data )
{
	CProtocol * me = (CProtocol*)data;
	if (me->IsListening())
	{
		if (strncmp(msg,"!msg ",5)==0)
		{
			return me->ParseMsg(msg,len);
		}
		else
		{
			LOG(ERR,"CProtocol::ArduinoListener: Ignoring [%s]",msg);
			return false;
		}
	}
	return false;
}

CProtocol::CProtocol(int ProtocolID)
{
	protocolID = ProtocolID;

	// we are using J2534-2 PIN switching mode
	if (protocolID & 0x8000)
		SetPinSwitched(true);

	_dummy_filter_id = 0xf1f10001;
	rollingPeriodicMsgId = 0xbebe0001;

	rxBufferSize = 0;
	rxBufferOverflow = false;
	hiIndex=lowIndex=0;
	datarate=SARDINE_DEFAULT_CAN_BAUD_RATE;
	listening = false;
	loopback = false;
	Arduino::RegisterListener((LPARDUINOLISTENER)ArduinoListener, this);
	periodicMsgHandler = NULL;
	interceptor = NULL;
}

CProtocol::~CProtocol(void)
	{
	Arduino::RemoveListener((LPARDUINOLISTENER)ArduinoListener);
	if (periodicMsgHandler)
		delete periodicMsgHandler;
	delete interceptor;
	}

// callback from CPeriodicMsgCallback, when timer has gone off in one of CPeriodicMsg instances
int CProtocol::SendPeriodicMsg( PASSTHRU_MSG * pMsg, unsigned long Id )
{
	LOG(PROTOCOL,"CProtocol::SendPeriodicMsg: msg id 0x%x",Id);
	if (!IsConnected())
		{
		LOG(ERR,"CProtocol::SendPeriodicMsg - not connected!");
		return ERR_DEVICE_NOT_CONNECTED;
		}
	// Write message (blocking)
	return WriteMsg(pMsg,0);
}


// Arduino protocol msg format: ":msg flags [01 02 03 .... 0x]'
bool  CProtocol::ParseMsg( char * msg, int len )
{
	char * bracket_open = strchr(msg,'[');
	char * bracket_close = strrchr(msg,']');
	if (bracket_open && bracket_close && (bracket_open<bracket_close) && len>10)
	{
		// calculate the 'flags' portion of message
		int flagslen = bracket_open-(&msg[5])-1;
		char flags[32+1];
		if (flagslen>32)
		{
		LOG(ERR,"CProtocol::ParseMsg: flagslen %d>32, truncating to 32",flagslen);
		flagslen=32;
		}
		strncpy_s(flags,33,&msg[5],flagslen);
//		char * flags = &msg[5];

		PASSTHRU_MSG * pmsg = new PASSTHRU_MSG;
		memset(pmsg,0,sizeof(PASSTHRU_MSG));
		pmsg->Timestamp = GetTime();

		int dataIndex=0;
		unsigned int databyte=0;
		char *p = bracket_open+1;
		while ( (p < bracket_close) && (databyte!=-1) )
			{
			databyte = convert_hex_to_int(p,2);
			if (databyte != -1)
				{
				pmsg->Data[dataIndex++] = databyte;
				}
			else
				{
				LOG(ERR,"CProtocol::ParseMsg: Invalid content in msg! [%s]",msg);
				delete pmsg;
				return false;
				}
			p += 3;
			}
		pmsg->DataSize = dataIndex;

		if (HandleMsg(pmsg,flags))
			{
			LOG(PROTOCOL,"CProtocol::ParseMsg: Message accepted - adding to rx buffer");
			LogMessage(pmsg,RECEIVED,channelId,"");
			AddToRXBuffer(pmsg);
//			NewMessageNotification(pmsg);
			}
		else
			{
			LOG(PROTOCOL,"CProtocol::ParseMsg: Message ignored");
			if ( (pmsg->ProtocolID==ISO15765) || (pmsg->ProtocolID == ISO15765_PS) )
				{
				LogMessage(pmsg,ISO15765_RECV,channelId," ignored before final assembly");
				// ISO15765 handler copied the contents from this msg. We can delete this, since we don't push it to rxbuffer 
				delete pmsg;
				}
			else
				{
				// not handled by this protocol, do not log.
				delete pmsg;
				return false;
				}
			}

	} else
	{
		LOG(ERR,"CProtocol::ParseMsg: Not valid Arduino protocol msg: [%s]",msg);
		return false;
	}
	return true;
}

int CProtocol::SendInterceptorMessage( PASSTHRU_MSG * pMsg )
{
	// put it to rxbuffer directly
	return AddToRXBuffer(pMsg);

	// FIXME: note that if we want to send ISO15765 message independently here, but want the higher level ISO15765 stack to put them together as a whole message, then we 
	// need to use HandleMsg and use correct flags..
//	HandleMsg(pMsg,"flagshere",...
}


int CProtocol::DoWriteMsg( PASSTHRU_MSG * pMsg, char * flags, unsigned long Timeout )
{
	LOG(PROTOCOL,"CProtocol::DoWriteMsg - timeout %d",Timeout);
	LOG(ERR,"CProtocol::DoWriteMsg  --- FIXME -- We ignore Timeout for now - call will be blocking");

	dbug_printmsg(pMsg,_T("Msg"),1,true);

	if ( (interceptor) && (interceptor->DoesMatchInterceptorFilter(pMsg)) )
	{
		LOG(PROTOCOL,"CProtocol::DoWriteMsg - message intercepted - NOT SENT!");
		return STATUS_NOERROR;
	}
	else
	{
		char buf[256];		
		unsigned int j=0;
		for (j=0;j<pMsg->DataSize;j++)
			sprintf_s(&buf[j*3],256-j*3,"%02x ",pMsg->Data[j]);
		int msg_len = j*3 -1;
		buf[msg_len]=0;

		// Delegate message sending to lower level
		if (SendMsg(buf,flags)!=(msg_len+strlen(flags)))
		{
			LOG(ERR,"CProtocolCAN::DoWriteMsg - sending message failed!");
			return ERR_FAILED;
		} else
		{
		LogMessage(pMsg,SENT,channelId,"");
		}
	}
	return STATUS_NOERROR;
}
	

int CProtocol::ProtocolID()
{
	return protocolID;
}


void CProtocol::SetPinSwitched( bool pinSwitchedMode )
{
	pinSwitched = pinSwitchedMode;
}

bool CProtocol::IsPinSwitched()
{
	return pinSwitched;
}

int CProtocol::StopPeriodicMessages()
{
	LOG(PROTOCOL,"CProtocol::StopPeriodicMessages");
	if (periodicMsgHandler)
		periodicMsgHandler->RemoveAllPeriodicMessages();
	else
		return ERR_FAILED;
	return STATUS_NOERROR;
}

int CProtocol::DeleteFilters()
{
	Arduino::Send(":deletefilters");
	return 0;
}


int CProtocol::SetDatarate( unsigned long rate )
{
	LOG(PROTOCOL,"CProtocol::SetDatarate: %d",datarate);
	LOG(ERR,"CProtocol::SetDatarate: --- does not actually set the datarate on device! FIXME!");
	datarate = rate;
	return STATUS_NOERROR;
}

int CProtocol::GetDatarate( unsigned long * rate)
{
	LOG(PROTOCOL,"CProtocol::GetDatarate: %d",datarate);
	LOG(ERR,"CProtocol::GetDatarate: --- does not actually retrive datarate from device! FIXME!");
	*rate = datarate;
	return STATUS_NOERROR;
}

bool CProtocol::IsListening()
{
	return listening;
}

void CProtocol::SetToListen( bool listen )
{
	listening=listen;
}


int CProtocol::SendMsg( char * msg, char * flags )
	{
	char buf[256];
	int len = strlen(msg) + strlen(flags) + 9;
	if (len>256)
		{
		LOG(ERR,"CProtocol::SendMsg: Too long msg! %d",len);
		return -1;
		}
	sprintf_s(buf,256,":msg %s [%s]",flags,msg);
	return Arduino::Send(buf) - 8;  // remove the decoration size
	}


void CProtocol::ClearRXBuffer()
	{
	LOG(PROTOCOL,"CProtocol::ClearRXBuffer");
	rx_lock.Lock();
	rxBufferOverflow=false;
	lowIndex=0;
	hiIndex=0;
	rxBufferSize=0;
	rx_lock.Unlock();
	}

void CProtocol::ClearTXBuffer()
	{
	LOG(PROTOCOL,"CProtocol::ClearTXBuffer -- FIXME: not implemented!---");
	}

int CProtocol::DoAddToRXBuffer( PASSTHRU_MSG * pMsg )
{
	if (rxBufferSize>=MAX_RX_BUFFER_SIZE)
		{
		LOG(ERR,"CProtocol::DoAddToRXBuffer -- buffer overflow!---");
		rxBufferOverflow=true;

		// we get rid of our oldest message
		PASSTHRU_MSG * oldMsg = rxbuffer[lowIndex];
		delete oldMsg;
		lowIndex++;	
		lowIndex %= MAX_RX_BUFFER_SIZE;
		
		rxbuffer[hiIndex] = pMsg;
		hiIndex++;
		hiIndex %= MAX_RX_BUFFER_SIZE;
		return ERR_BUFFER_OVERFLOW;
		} 
	else
		{
		rxbuffer[hiIndex] = pMsg;
		hiIndex++;
		hiIndex %= MAX_RX_BUFFER_SIZE;
		rxBufferSize++;
		}
	return STATUS_NOERROR;
}

int CProtocol::AddToRXBuffer( PASSTHRU_MSG * pMsg )
{
	LOG(PROTOCOL,"CProtocol::AddToRXBuffer");
	rx_lock.Lock();
	unsigned int ret = DoAddToRXBuffer(pMsg);
	rx_lock.Unlock();
	return ret;
}


void CProtocol::SetRXBufferOverflow(bool status)
{
	rx_lock.Lock();
	LOG(HELPERFUNC,"CProtocol::SetRXBufferOverflow %d",status);
	rxBufferOverflow = status;
	rx_lock.Unlock();
}

int CProtocol::GetRXMessageCount()
{
	rx_lock.Lock();
	LOG(HELPERFUNC,"CProtocol::GetRXMessageCount: %d",rxBufferSize);
	assert( rxBufferSize == ((hiIndex-lowIndex)%MAX_RX_BUFFER_SIZE));	// sanity check
	int s = rxBufferSize;
	rx_lock.Unlock();
	return s;
}

PASSTHRU_MSG * CProtocol::PopMessage()
{
	LOG(PROTOCOL,"CProtocol::PopMessage");
	rx_lock.Lock();
	PASSTHRU_MSG * msg = rxbuffer[lowIndex++];
	lowIndex %= MAX_RX_BUFFER_SIZE;
	rxBufferSize--;
	rxBufferOverflow=false;
	rx_lock.Unlock();
	return msg;
}

bool CProtocol::IsRXBufferOverflow() 
{
	rx_lock.Lock();
	LOG(HELPERFUNC,"CProtocol::IsBufferOverflow");
	bool rxoverflow=rxBufferOverflow;
	rx_lock.Unlock();
	return rxoverflow;
}

bool CProtocol::IsConnected()
{
	return Arduino::IsConnected();
}



int CProtocol::AddLoopbackMsg( PASSTHRU_MSG * pMsg )
	{
	LOG(MAINFUNC,"CProtocol::AddLoopbackMsg");
	PASSTHRU_MSG * pLoopBackMsg = new PASSTHRU_MSG;
	if (pLoopBackMsg==NULL)
		return ERROR_OUTOFMEMORY;
	memset(pLoopBackMsg,0,sizeof(PASSTHRU_MSG));
	pLoopBackMsg->ProtocolID = pMsg->ProtocolID;
	pLoopBackMsg->RxStatus = pMsg->RxStatus | TX_MSG_TYPE;	// set as succesfully transmitted
	pLoopBackMsg->Timestamp = GetTime();
	pLoopBackMsg->DataSize = pMsg->DataSize;
	// pLoopBackMsg->TxFlags= pMsg->TxFlags  // do we need this?

	for (unsigned int i=0;i<pMsg->DataSize;i++)
		pLoopBackMsg->Data[i] = pMsg->Data[i];
	pLoopBackMsg->ExtraDataIndex = pMsg->ExtraDataIndex;	// we don't have any checksum
	LOG(MAINFUNC,"CProtocol::AddLoopbackMsg -- adding to rx msg queue");
	return AddToRXBuffer(pLoopBackMsg);	// gets ownership 
	}

int CProtocol::WriteMsgs( PASSTHRU_MSG * pMsgs, unsigned long * pNumMsgs, unsigned long Timeout )
{
	LOG(PROTOCOL,"CProtocol::WriteMsgs - num_msgs: %d, timeout %d",*pNumMsgs, Timeout);
	unsigned int err;

	if (!IsConnected())
		{
		LOG(ERR,"CProtocol::WriteMsgs - not connected");
		return ERR_DEVICE_NOT_CONNECTED;
		}

	if (*pNumMsgs > MAX_J2534_MESSAGES)
		{
		LOG(ERR,"CProtocol::WriteMsgs - tried sending too many messages (according to specs limit is 10)");
		return ERR_EXCEEDED_LIMIT;
		}

	for (unsigned int i=0;i<*pNumMsgs;i++)
	{
		PASSTHRU_MSG * curr_msg = &pMsgs[i];
		if (curr_msg->ProtocolID != ProtocolID() ) 
		{
#ifndef ENFORCE_PROTOCOL_IDS_IN_MSGS
			LOG(ERR,"CProtocol::WriteMsgs - invalid protocol id in message #%d: %d != %d (expected protocol) -- ignoring for now",i,curr_msg->ProtocolID,ProtocolID());
#else
			LOG(ERR,"CProtocol::WriteMsgs - invalid protocol id in message #%d: %d != %d (expected protocol)",i,curr_msg->ProtocolID,ProtocolID());
			return ERR_MSG_PROTOCOL_ID;
#endif
		}
		err=WriteMsg(curr_msg,Timeout);
		if (err!=STATUS_NOERROR)
			{
			LOG(ERR,"CPRotocol::WriteMsgs - error while writing msg -> aborting!");
			return err;
			}

		// FIXME: since we now use blocking write, we can dispatch loopback messages immediately. MUST be changed when using ISO 15765 flow control! (added to rx queue only after
		// sending all the sub-messages)
		if (IsLoopback())
			AddLoopbackMsg(curr_msg);
	
	}
	LOG(PROTOCOL_VERBOSE,"CProtocol::WriteMsgs: writing %d messages successful!",*pNumMsgs);
	return STATUS_NOERROR;
}


int CProtocol::GetIOCTLParam( SCONFIG * pConfig )
{
	LOGW(PROTOCOL,_T("CProtocol::GetIOCTLParam - parameter %d [%s]"), pConfig->Parameter,dbug_param2str(pConfig->Parameter));
	switch(pConfig->Parameter)
		{
		case DATA_RATE:
			GetDatarate( &pConfig->Value );
			return STATUS_NOERROR;
		case LOOPBACK:
			pConfig->Value = IsLoopback();
			LOG(PROTOCOL,"CProtocol::GetIOCTLParam -get loopback: %d",pConfig->Value);
			return STATUS_NOERROR;
		case J1962_PINS:
			LOG(PROTOCOL,"CProtocol::SetIOCTLParam - setting J1962 pins: pin1: %d, pin2: %d",(pConfig->Value>>8)&0xFF, pConfig->Value&0xFF);
			return SetJ1962Pins((pConfig->Value>>8)&0xFF, pConfig->Value&0xFF);
		default:
			LOG(ERR,"CProtocol::GetIOCTLParam - Parameter not supported ! --- FIXME?");
			return ERR_NOT_SUPPORTED;
		}

	return STATUS_NOERROR;
}


int CProtocol::SetIOCTLParam( SCONFIG * pConfig )
{
	LOGW(PROTOCOL,_T("CProtocol::SetIOCTLParam - parameter %d [%s]"), pConfig->Parameter,dbug_param2str(pConfig->Parameter));
	switch(pConfig->Parameter)
		{
		case DATA_RATE:
#ifdef PLAY_STUPID
			LOG(ERR,"CProtocol::SetIOCTLParam - ignoring set DATA_RATE --- FIXME");
			return STATUS_NOERROR;
#else
			LOG(ERR,"CProtocol::SetIOCTLParam - unsupported parameter! --- FIXME");
			return ERR_NOT_SUPPORTED;
#endif
		case LOOPBACK:
			if (pConfig->Value==0)
				{
				LOG(PROTOCOL,"CProtocol::SetIOCTLParam - disabling loopback");
				}
			else
				{
				LOG(PROTOCOL,"CProtocol::SetIOCTLParam - enabling loopback");
				}
			SetLoopback( (pConfig->Value==1) ? true : false );
			return STATUS_NOERROR;
		case J1962_PINS:
			LOG(PROTOCOL,"CProtocol::SetIOCTLParam - setting J1962 pins: pin1: %d, pin2: %d",(pConfig->Value>>8)&0xFF, pConfig->Value&0xFF);
			return SetJ1962Pins((pConfig->Value>>8)&0xFF, pConfig->Value&0xFF);
		default:
			LOG(ERR,"CProtocol::SetIOCTLParam - Parameter not supported! --- FIXME?");
			return ERR_NOT_SUPPORTED;
		}

	return STATUS_NOERROR;
}



int CProtocol::IOCTL(unsigned long IoctlID, void *pInput, void *pOutput)
{
	LOG(PROTOCOL,"CProtocol::IOCTL - ioctl command %d", IoctlID);
	unsigned int err;

	switch (IoctlID)
	{
	case GET_CONFIG:
		{
		if (pInput==NULL)
			{
			LOG(ERR,"CProtocol::IOCTL: pInput==NULL!");
			return ERR_NULL_PARAMETER;
			}
		SCONFIG_LIST * pList = (SCONFIG_LIST*)pInput;
		for (unsigned int i=0;i<pList->NumOfParams;i++)
			{
			SCONFIG * pConfig = &pList->ConfigPtr[i];
			if (pConfig==NULL)
				{
				LOG(ERR,"CProtocol::IOCTL: pConfig==NULL!");
				return ERR_NULL_PARAMETER;
				}
			err=GetIOCTLParam(pConfig);
			if (err!=STATUS_NOERROR)
				{
				LOG(ERR,"CProtocol::IOCTL: exiting !");
				return err;
				}
			}
		break;
		}
	case SET_CONFIG:
		{
		if (pInput==NULL)
			{
			LOG(ERR,"CProtocol::IOCTL: pInput==NULL!");
			return ERR_NULL_PARAMETER;
			}
		SCONFIG_LIST * pList = (SCONFIG_LIST*)pInput;
		for (unsigned int i=0;i<pList->NumOfParams;i++)
			{
			SCONFIG * pConfig = &pList->ConfigPtr[i];
			if (pConfig==NULL)
				{
				LOG(ERR,"CProtocol::IOCTL: pConfig==NULL!");
				return ERR_NULL_PARAMETER;
				}
			err=SetIOCTLParam(pConfig);
			if (err!=STATUS_NOERROR)
				{
				LOG(ERR,"CProtocol::IOCTL: exiting !");
				return err;
				}
		}
		break;
		}
	case CLEAR_TX_BUFFER:
		ClearTXBuffer();
		break;
	case CLEAR_RX_BUFFER:
		ClearRXBuffer();
		break;
	case CLEAR_PERIODIC_MSGS:
		return StopPeriodicMessages();
		break;
	case CLEAR_MSG_FILTERS:
		return DeleteFilters();
		break;
	default:
		LOG(MAINFUNC,"CProtocol::IOCTL----- NOT SUPPORTED -----");
		return ERROR_NOT_SUPPORTED;
		break;

	}

	return STATUS_NOERROR;
}


void CProtocol::SetLoopback( bool _loopback )
{
	rx_lock.Lock();
	loopback=_loopback;
	rx_lock.Unlock();
}

bool CProtocol::IsLoopback()
{
	rx_lock.Lock();
	bool _loopback=loopback;
	rx_lock.Unlock();
	return _loopback;
}


int CProtocol::SetJ1962Pins( unsigned long pin1, unsigned long pin2 )
{
	LOG(PROTOCOL,"CPRotocol::SetJ1962Pins - pin1: %d, pin2: %d", pin1, pin2);
#ifdef PLAY_STUPID
	LOG(ERR,"CPRotocol::SetJ1962Pins - pin switching not really implemented, just saving given values");
	J1962Pin1 = pin1;
	J1962Pin2 = pin2;
	return STATUS_NOERROR;
#else	
	LOG(ERR,"CPRotocol::SetJ1962Pins - pin switching not supported");
	return ERR_NOT_SUPPORTED;
#endif
}

int CProtocol::GetJ1962Pins( unsigned long * pin1, unsigned long * pin2 )
{
	LOG(PROTOCOL,"CPRotocol::GetJ1962Pins - pin1: %d, pin2: %d", J1962Pin1, J1962Pin2);
#ifdef PLAY_STUPID
	LOG(ERR,"CPRotocol::SetJ1962Pins - pin switching not really implemented, just replying with saved values");
	*pin1 = J1962Pin1;
	*pin2 = J1962Pin2;
	return STATUS_NOERROR;
#else	
	LOG(ERR,"CPRotocol::SetJ1962Pins - pin switching not supported");
	return ERR_NOT_SUPPORTED;
#endif
}


int CProtocol::Connect(unsigned long _channelId, unsigned long Flags)
{
	LOG(PROTOCOL,"CProtocol::Connect - flags: 0x%x",Flags);

	channelId = _channelId;
	periodicMsgHandler = new CPeriodicMessageHandler();
	if (periodicMsgHandler==NULL)
		return ERR_FAILED;
	if (!periodicMsgHandler->createMsgHandlerThread(channelId))
	{
		delete periodicMsgHandler;
		periodicMsgHandler=NULL;
		return ERR_FAILED;
	}
	
	if (CInterceptor::UseInterceptor())
	{
		interceptor = new CInterceptor(this,protocolID);
	}

	// we are now receiving messages via HandleMsg
	SetToListen(true);

	// Delete existing message filters for this protocol and stop periodic messages, as specified in J2534-1
	DeleteFilters();
	StopPeriodicMessages();

	return STATUS_NOERROR;
}

int CProtocol::Disconnect() 
{
	LOG(PROTOCOL,"CProtocol::Disconnect");

	// Delete existing message filters for this protocol and stop periodic messages, as specified in J2534-1
	StopPeriodicMessages();
	DeleteFilters();

	delete periodicMsgHandler;
	periodicMsgHandler=NULL;

	return STATUS_NOERROR;
}

int CProtocol::DoReadMsgs( PASSTHRU_MSG * pMsgs, unsigned int count, bool overflow )
{
	for (unsigned int i=0;i<count;i++)
		{
		LOG(PROTOCOL_VERBOSE,"CProtocol::DoReadMsg - reading msg #%d",i);
		PASSTHRU_MSG * curr_msg = PopMessage();
		dbug_printmsg(curr_msg,_T("ReadMsg"),1,false);
		PASSTHRU_MSG * pDestMsg = &pMsgs[i];
		LOG(PROTOCOL_VERBOSE,"CProtocol::DoReadMsg - copying msg #%d",i);
		memcpy(pDestMsg,curr_msg,sizeof(PASSTHRU_MSG));
		}
	if (overflow)
		{
		LOG(ERR,"CProtocol::ReadMsgs: we had buffer overflow");
		return ERR_BUFFER_OVERFLOW;
		}
	else
		{
		LOG(PROTOCOL,"CProtocol::DoReadMsgs success");
		return STATUS_NOERROR;
		}
}

int CProtocol::ReadMsgs( PASSTHRU_MSG * pMsgs, unsigned long * pNumMsgs, unsigned long Timeout )
{
	LOG(PROTOCOL,"CProtocol::ReadMsgs: timeout %d",Timeout);

	unsigned long count=GetRXMessageCount();
	if ( (count==0) && (Timeout==0) )
		{
		LOG(PROTOCOL_VERBOSE,"CProtocol::ReadMsgs: No messages and timeout==0 " );
		*pNumMsgs = 0;
		return ERR_BUFFER_EMPTY;
		}

	bool overflow = IsRXBufferOverflow();
	if (count>=(*pNumMsgs))
		{
		LOG(PROTOCOL_VERBOSE,"CProtocol::ReadMsgs: Enough messages in buffer: sending %d msgs",*pNumMsgs);
		// we have (more than) enough of buffered messages, read just the amount requested
		return DoReadMsgs(pMsgs,*pNumMsgs,overflow);
		}
	else if ( (Timeout==0) && (count>0) )
		{
		LOG(PROTOCOL_VERBOSE,"CProtocol::ReadMsgs: timeout==0 and we have >0 msgs in buffer: sending %d msgs",count);
		*pNumMsgs = count;
		return DoReadMsgs(pMsgs,count,overflow);
		}
	else
		{
		LOG(PROTOCOL_VERBOSE,"CProtocol::ReadMsgs: sleeping for %d milliseconds while waiting for new messages",Timeout);
		// sleep here
		SleepEx( Timeout,FALSE );	// FIXME: need better time handling: sleep MAXIMUM of timeout, in case enough messages will come before that
		count=GetRXMessageCount();
		if (count>0)
			{
			// read MIN(*pNumMsgs,count) messages
			*pNumMsgs = (count > *pNumMsgs) ? *pNumMsgs : count;
			return DoReadMsgs(pMsgs,*pNumMsgs,overflow);
			}
		else
			return ERR_BUFFER_EMPTY;
		}

	// we shouldn't reach this
	assert(0);
	return ERR_NOT_SUPPORTED;
}


int CProtocol::StartMsgFilter( unsigned long FilterType, PASSTHRU_MSG * pMaskMsg, PASSTHRU_MSG * pPatternMsg, PASSTHRU_MSG * pFlowControlMsg, unsigned long * pFilterID )
{
	LOGW(PROTOCOL,_T("CProtocol::StartMsgFilter - filtertype %s"), dbug_filter2str(FilterType));
	
	dbug_printmsg(pMaskMsg,_T("MaskMsg"),1,true);
	dbug_printmsg(pPatternMsg,_T("PatternMsg"),1,true);
	if (pFlowControlMsg)
		dbug_printmsg(pFlowControlMsg,_T("FlowControlMsg"),1,true);

	if (pMaskMsg->ProtocolID != ProtocolID())
	{
		LOG(ERR,"CProtocol::StartMsgFilter: invalid protocol ID %d in MaskMsg",pMaskMsg->ProtocolID);
		return ERR_MSG_PROTOCOL_ID;
	}
	if (pPatternMsg->ProtocolID != ProtocolID())
	{
		LOG(ERR,"CProtocol::StartMsgFilter: invalid protocol ID %d in MaskMsg",pPatternMsg->ProtocolID);
		return ERR_MSG_PROTOCOL_ID;
	}
	if (pMaskMsg->DataSize > 12)
	{
		LOG(ERR,"CProtocol::StartMsgFilter: data length %d > 12 in pMaskMsg",pMaskMsg->DataSize);
		return ERR_INVALID_MSG;
	}
	if (pPatternMsg->DataSize > 12)
	{
		LOG(ERR,"CProtocol::StartMsgFilter: data length %d > 12 in pPatternMsg",pPatternMsg->DataSize);
		return ERR_INVALID_MSG;
	}
	if (pPatternMsg->DataSize != pMaskMsg->DataSize)
	{
		LOG(ERR,"CProtocol::StartMsgFilter: inequal datasizes in pattern(%d) and mask msg (%d) !",pPatternMsg->DataSize,pMaskMsg->DataSize);
		return ERR_INVALID_MSG;
	}
	if (pPatternMsg->TxFlags != pMaskMsg->TxFlags)
	{
		LOG(ERR,"CProtocol::StartMsgFilter: inequal TxFlags in pattern (0x%x) and mask msg (0x%x) !",pPatternMsg->TxFlags,pMaskMsg->TxFlags);
		return ERR_INVALID_MSG;
	}


	// clearing buffer so that it doesn't contain any messages that might conflict with this filter
	ClearRXBuffer();

#ifdef PLAY_STUPID
	LOG(ERR,"CProtocol::StartMsgFilter - not yet implemented, ignored");
	*pFilterID=_dummy_filter_id++;
	return STATUS_NOERROR;
#else
	LOG(ERR,"CProtocol::StartMsgFilter  --- FIXME ! does not exist");
	return ERR_NOT_SUPPORTED;
#endif
}

int CProtocol::StopMsgFilter(  unsigned long FilterID )
{
	LOG(PROTOCOL,"CProtocol::StopMsgFilter - filter id 0x%x", FilterID);
#ifdef PLAY_STUPID
	LOG(ERR,"CProtocol::StopMsgFilter - not yet implemented, ignored");
	return STATUS_NOERROR;
#else
	LOG(ERR,"CProtocol::StopMsgFilter  --- FIXME ! does not exist");
	return ERR_NOT_SUPPORTED;
#endif
}


int CProtocol::StartPeriodicMsg( PASSTHRU_MSG * pMsg, unsigned long * pMsgID, unsigned long TimeInterval)
{
	LOG(PROTOCOL,"CProtocol::StartPeriodicMsg - time interval %d milliseconds", TimeInterval);
	dbug_printmsg(pMsg,_T("PeriodicMsg"),1,true);

	if (!IsConnected())
		{
		LOG(ERR,"CProtocol::StartPeriodicMsg - not connected");
		return ERR_DEVICE_NOT_CONNECTED;
		}

	if (pMsg->ProtocolID != ProtocolID() ) 
	{
#ifdef ENFORCE_PROTOCOL_IDS_IN_MSGS
		LOG(ERR,"CProtocol::WriteMsgs - invalid protocol id: %d != %d (expected protocol) -- ignoring for now",pMsg->ProtocolID,ProtocolID());
#else
		LOG(ERR,"CProtocol::WriteMsgs - invalid protocol id: %d != %d (expected protocol)",pMsg->ProtocolID,ProtocolID());
		return ERR_MSG_PROTOCOL_ID;
#endif
	}

	
	CPeriodicMsg * msg = new CPeriodicMsg(this,rollingPeriodicMsgId,TimeInterval);
	if (msg==NULL)
		return ERR_FAILED;
	if (msg->AttachMessage(pMsg))
		{
		delete msg;
		return ERR_FAILED;
		}

	int ret;
	if ((ret=periodicMsgHandler->AddPeriodicMessage(msg))!=STATUS_NOERROR)
		{
		delete msg;
		return ret;
		}


	*pMsgID = rollingPeriodicMsgId++;

		 return STATUS_NOERROR;
}

int CProtocol::StopPeriodicMsg( unsigned long MsgID)
{
	LOG(PROTOCOL,"CProtocol::StopPeriodicMsg - msg id 0x%x",MsgID);
	int ret= periodicMsgHandler->RemovePeriodicMessage(MsgID);
	return ret;
}
