#include "stdafx.h"
#include "ProtocolISO15765.h"
#include "helper.h"


CProtocolISO15765::CProtocolISO15765(int ProtocolID)
	:CProtocolCAN(ProtocolID)
{
	can_mixed_format=0; //off
	_running_filter_id = 0;
	sessionsnum = 0;
	filtersnum = 0;

	block_size = 0;	// default: A value of zero allows the remaining frames to be sent without flow control or delay
	separation_time = 0; // default: Separation Time (ST), the minimum delay time between frames
}


CProtocolISO15765::~CProtocolISO15765(void)
{
}

bool CProtocolISO15765::GenerateFlowControlMsg( flowFilter * filter, unsigned int flowStatus )
{
	LOG(PROTOCOL,"CProtocolISO15765::GenerateFlowControlMsg: flowStatus %d",flowStatus);
	PASSTHRU_MSG * msg = new PASSTHRU_MSG;
	memset(msg,0,sizeof(PASSTHRU_MSG));
	msg->ProtocolID = ProtocolID();
	for (int i=0;i<filter->len;i++)
		msg->Data[i] = filter->flowControlMsg[i];
	char PCI = ISO15765_PCI_FLOW_CONTROL_FRAME | flowStatus; // flow control message type in high nibble, flow status in low nibble
	if (filter->ext_addr)
	{
		msg->TxFlags |= ISO15765_ADDR_TYPE;
		msg->Data[5] = PCI;	// in extended addressing 6th byte is the PCI
	}
	else
	{
		msg->Data[4] = PCI;	// without ext.addressing 5th byte is the PCI
	}
	msg->DataSize = filter->len + 1;

	// send the FC message
	if (CProtocolCAN::WriteMsg(msg,0) != STATUS_NOERROR)
	{
		LOG(PROTOCOL,"CProtocolISO15765::GenerateFlowControlMsg: Failed sending FC message! Will not create session, ignoring this msg!");
		return false;
	}
	return true;
}
	
int CProtocolISO15765::GetSessionByMsg( PASSTHRU_MSG * pMsg )
{
	unsigned int i=0;
	while (i<sessionsnum)
	{
		if ( (pMsg->Data[0]==sessions[i].msg->Data[0]) &&
			(pMsg->Data[1]==sessions[i].msg->Data[1]) &&
			(pMsg->Data[2]==sessions[i].msg->Data[2]) &&
			(pMsg->Data[3]==sessions[i].msg->Data[3]) )
			{
				// possible match. Still have to check the extended address byte
				if (pMsg->TxFlags & ISO15765_EXT_ADDR)
				{
					// does the extended addr match?
					if (pMsg->Data[4]==sessions[i].msg->Data[4])
						return i;
				} else
				{
					// no extended addressing. Match.
					return i;
				}
		}
		i++;
	}
	return -1;
}

void CProtocolISO15765::DeleteSession( unsigned int sessionIdx )
{
	if (sessionIdx<sessionsnum)
	{
	if (sessions[sessionIdx].msg)
		delete sessions[sessionIdx].msg;
		for (unsigned int i=sessionIdx;i<sessionsnum-1;i++)
			{
			sessions[i] = sessions[i+1];
			}
	} else
	{
	LOG(ERR,"CProtocolISO15765::DeleteSession: Invalid session id %d",sessionIdx);
	}
}

bool CProtocolISO15765::HandleIncomingFlowControlMessage( PASSTHRU_MSG * pMsg )
{
	LOG(ERR,"CProtocolISO15765::HandleIncomingFlowControlMessage: not implemented yet! --- FIXME!");
	return false;
}


bool CProtocolISO15765::HandleConsecutiveFrame( PASSTHRU_MSG * pMsg, unsigned char PCI )
{
	LOG(PROTOCOL,"CProtocolISO15765::HandleConsecutiveFrame: length: %d",pMsg->DataSize);
	bool lastFrame=false;
	int sidx = GetSessionByMsg(pMsg);
	if (sidx==-1)
	{
		LOG(ERR,"CProtocolISO15765::HandleConsecutiveFrame: No associated FC session with current msg found! -> ignoring msg ");
		return false;
	}
	unsigned char seqNumber = PCI & 0x0F; // sequence number in low nibble of PCI
	unsigned char expectedSeqNumber = (sessions[sidx].previousFrameSeqNumber + 1) % 16;
	// FIXME: assemble message based on sequence number! Here we just blindly append bytes ignoring the sequence 
	if (seqNumber != expectedSeqNumber)
	{
		LOG(ERR,"CProtocolISO15765::HandleConsecutiveFrame: unexpected seq number! Previous %d, current %d - FIXME: ignoring silently ",sessions[sidx].previousFrameSeqNumber,seqNumber);
	}
	sessions[sidx].previousFrameSeqNumber = seqNumber;
	int remainingBytes = sessions[sidx].expected_length - sessions[sidx].payloadLen;
	int availableBytes = pMsg->DataSize - 5;
	if (sessions[sidx].extended_addressing)
		availableBytes--;

	if (remainingBytes==availableBytes)
	{
		lastFrame=true;
	}

	if (remainingBytes<availableBytes)	
	{
		LOG(ERR,"CProtocolISO15765::HandleConsecutiveFrame: remainingBytes<availableBytes, something wrong with our code? ");
		// copy only the remaining ones, rest could be padding
		availableBytes=remainingBytes;
		lastFrame=true;
	}

	// copy the available bytes
	for (int i=0;i<availableBytes;i++)
		{
			sessions[sidx].msg->Data[sessions[sidx].msg->DataSize++] = pMsg->Data[i];
			sessions[sidx].payloadLen++;
		}

	if (lastFrame)
	{
		LOG(PROTOCOL,"CProtocolISO15765::HandleConsecutiveFrame: last frame! dispatching!");
		AddToRXBuffer( sessions[sidx].msg );
		sessions[sidx].msg=NULL;
		DeleteSession(sidx);
	}
	return true;
}

// Start new session, move received data from current msg to a session buffer, and place a RXStart indication msg to rx-buffer instead of this message
// This is done by modifying the current message (so not really sending a whole new message)
void CProtocolISO15765::StartSession( PASSTHRU_MSG * pFirstMsg )
{
	LOG(PROTOCOL,"CProtocolISO15765::StartSession: First frame");
	PASSTHRU_MSG * msg = new PASSTHRU_MSG;
	unsigned int i = 0;
	memset(msg,0,sizeof(PASSTHRU_MSG));	
	sessions[sessionsnum].msg = msg;
	sessions[sessionsnum].previousFrameSeqNumber=0;	// sequence numbering starts at 0 for first message
	sessions[sessionsnum].extended_addressing = ((pFirstMsg->RxStatus & ISO15765_ADDR_TYPE) ? true : false);

	// first copy the address (+extended address)
	sessions[sessionsnum].headerLen = (sessions[sessionsnum].extended_addressing ? 5 : 4);
	for (unsigned i=0;i<sessions[sessionsnum].headerLen;i++)
	{
		msg->Data[i] = pFirstMsg->Data[i];
	}

	// analyze the PCI bytes: total msg chunk length = low nibble of PCI1 (MSB part) + second PCI2 (LSB part)
	sessions[sessionsnum].expected_length = (((unsigned int)pFirstMsg->Data[i] & 0xF)<<8) | (pFirstMsg->Data[i+1]);
	// we omit copying the two PCI bytes
	i+=2;

	// then the payload
	sessions[sessionsnum].payloadLen = 0;
	for ( ;i<pFirstMsg->DataSize;i++)
	{
		msg->Data[i-2] = pFirstMsg->Data[i];
		sessions[sessionsnum].payloadLen++;
	}
	msg->DataSize = i-2;
	
	msg->RxStatus = pFirstMsg->RxStatus;
	msg->ExtraDataIndex = pFirstMsg->ExtraDataIndex;
	msg->ProtocolID = pFirstMsg->ProtocolID;

	// we set FIRST_FRAME bit and clean up the first message 
	pFirstMsg->DataSize = ((pFirstMsg->RxStatus & ISO15765_ADDR_TYPE) ? 5 : 4);
	pFirstMsg->RxStatus |= ISO15765_FIRST_FRAME;
	pFirstMsg->ExtraDataIndex = 0;

	sessionsnum++;
}



// new message arrived. Check if it needs flow control
bool CProtocolISO15765::HandleMsg( PASSTHRU_MSG * pMsg, char * flags)
{
	LOG(PROTOCOL_MSG,"CProtocolISO15765::HandleMsg: flags [%s]");
	// first we let the CAN level handle the flag setting etc.
	int ret = CProtocolCAN::HandleMsg( pMsg, flags);

	unsigned char PCI;
	if (pMsg->RxStatus & ISO15765_EXT_ADDR)
		PCI = pMsg->Data[5]; // 6th byte is PCI in extended addressing
	else
		PCI = pMsg->Data[4]; // 5th byte is PCI in normal addressing

	LOG(PROTOCOL,"CProtocolISO15765::HandleMsg: ext_addr: %d, PCI: 0x%x",(pMsg->RxStatus & ISO15765_EXT_ADDR)?1:0,PCI);

	unsigned char N_PCItype = PCI & 0xF0; // select the high nibble of PCI
	switch (N_PCItype) // 
	{
		case ISO15765_PCI_SINGLE_FRAME:
			{
			LOG(PROTOCOL,"CProtocolISO15765::HandleMsg: Single frame, dispatching normally");
			return true;
			}
		case ISO15765_PCI_FIRST_FRAME:
			{
			LOG(PROTOCOL,"CProtocolISO15765::HandleMsg: First frame");

			int filteridx = GetMatchingFilter(pMsg,false);
			if (filteridx==-1)
				{
				LOG(ERR,"CProtocolISO15765::HandleMsg: Msg without associated flow filter! Ignoring now, and sending as normal CAN msg..");
				return true;
				}

			// send CLEAR_TO_SEND message
			if (GenerateFlowControlMsg(&filters[filteridx],ISO15765_FS_CTS))
			{
				// Start new session, move received data from current msg to a session buffer, and place a RXStart indication msg to rx-buffer instead of this message
				// This is done by modifying the current message (so not really sending a whole new message)
				StartSession(pMsg);
				return true;
			} else
				return false;
			} 
		case ISO15765_PCI_CONSECUTIVE_FRAME:
			LOG(PROTOCOL,"CProtocolISO15765::HandleMsg: Consecutive frame, not yet dispatching (unless it's last frame)");
			HandleConsecutiveFrame(pMsg,PCI);
			return false;
		case ISO15765_PCI_FLOW_CONTROL_FRAME:
			HandleIncomingFlowControlMessage(pMsg);
			return false;
		default:
			LOG(ERR,"CProtocolISO15765::HandleMsg: Invalid PCI byte! Ignoring now, and sending as normal CAN msg..");
			return true;
	}
}


int CProtocolISO15765::WriteMsg( PASSTHRU_MSG * pMsg, unsigned long Timeout )
{
	LOG(PROTOCOL,"CProtocolISO15765::DoWriteMsg - timeout %d",Timeout);

	dbug_printmsg(pMsg,_T("ISO 15765 Msg"),1,true);

	if (!DoesMatchFilter(pMsg,true))
		{
		LOG(ERR,"CProtocolISO15765::DoWriteMsg - A flow control filter associated with this message (CAN id) not found! ");
		return ERR_NO_FLOW_CONTROL;
		}

	if (pMsg->DataSize>12) 
	{
		LOG(ERR,"CProtocolISO15765::DoWriteMsg - sending large messages (msg len %d) with flow control not yet implemented! failing! ",pMsg->DataSize);
		return ERR_NOT_SUPPORTED;
	} 


	if (pMsg->TxFlags & ISO15765_FRAME_PAD)
	{
		while (pMsg->DataSize < 12)
		{
		pMsg->Data[pMsg->DataSize++] = 0;
		}
	}

	// Delegate message sending to lower level
	return CProtocolCAN::WriteMsg(pMsg,Timeout);
}


int CProtocolISO15765::GetIOCTLParam( SCONFIG * pConfig )
{
	LOGW(PROTOCOL,_T("CProtocolISO15765::GetIOCTLParam - parameter %d [%s]"), pConfig->Parameter,dbug_param2str(pConfig->Parameter));
	switch(pConfig->Parameter)
		{
		case ISO15765_BS:
			LOG(PROTOCOL,"CProtocolISO15765::GetIOCTLParam - Querying ISO15765_BS ");
			pConfig->Value=block_size;
			return STATUS_NOERROR;
		case ISO15765_STMIN:
			LOG(PROTOCOL,"CProtocolISO15765::GetIOCTLParam - Querying ISO15765_STMIN");
			pConfig->Value=separation_time;
			return STATUS_NOERROR;
		case CAN_MIXED_FORMAT:
#ifdef PLAY_STUPID
			LOG(ERR,"CProtocolISO15765::GetIOCTLParam - CAN_MIXED_FORMAT not implemented, returning saved value: %d",can_mixed_format);
			pConfig->Value=can_mixed_format;
			return STATUS_NOERROR;
#else
			LOG(ERR,"CProtocolISO15765::GetIOCTLParam - CAN_MIXED_FORMAT not implemented -- FIXME!");
			return ERR_NOT_SUPPORTED;
#endif
		default:
			LOG(PROTOCOL,"CProtocolISO15765::GetIOCTLParam - reverting to base class handler");
			return CProtocolCAN::GetIOCTLParam(pConfig);
		}

	// never reach this
	return STATUS_NOERROR;
}


int CProtocolISO15765::SetIOCTLParam( SCONFIG * pConfig )
{
	LOGW(PROTOCOL,_T("CProtocolISO15765::SetIOCTLParam - parameter %d [%s]"), pConfig->Parameter,dbug_param2str(pConfig->Parameter));
	switch(pConfig->Parameter)
		{
		case ISO15765_BS:
			LOG(PROTOCOL,"CProtocolISO15765::SetIOCTLParam - Setting ISO15765_BS");
			block_size=pConfig->Value;
			return STATUS_NOERROR;
		case ISO15765_STMIN:
			LOG(PROTOCOL,"CProtocolISO15765::SetIOCTLParam - Setting ISO15765_STMIN");
			separation_time=pConfig->Value;
			return STATUS_NOERROR;
		case CAN_MIXED_FORMAT:
#ifdef PLAY_STUPID
			can_mixed_format = pConfig->Value;
			LOG(ERR,"CProtocolISO15765::SetIOCTLParam - CAN_MIXED_FORMAT not implemented, just saving given value: %d",can_mixed_format);
			return STATUS_NOERROR;
#else
			LOG(ERR,"CProtocolISO15765::SetIOCTLParam - CAN_MIXED_FORMAT not implemented -- FIXME!");
			return ERR_NOT_SUPPORTED;
#endif
		default:
			LOG(PROTOCOL,"CProtocolISO15765::SetIOCTLParam - reverting to base class handler");
			return CProtocolCAN::GetIOCTLParam(pConfig);
		}

	// never reach this
	return STATUS_NOERROR;
}

int CProtocolISO15765::DeleteFilters()
{
	LOG(PROTOCOL,"CProtocolISO15765::DeleteFilters");
	filtersnum=0;
	return CProtocol::DeleteFilters();
}

/*
int  CProtocolISO15765::IOCTL(unsigned long IoctlID, void *pInput, void *pOutput)
{
	LOG(PROTOCOL,"CProtocolISO15765::IOCTL - ioctl command %d", IoctlID);
	unsigned int err;

	switch (IoctlID)
	{
	case GET_CONFIG:
		{
		if (pInput==NULL)
			{
			LOG(ERR,"CProtocolISO15765::IOCTL: pInput==NULL!");
			return ERR_NULL_PARAMETER;
			}
		SCONFIG_LIST * pList = (SCONFIG_LIST*)pInput;
		for (unsigned int i=0;i<pList->NumOfParams;i++)
		{
			SCONFIG * pConfig = &pList->ConfigPtr[i];
			if (pConfig==NULL)
				{
				LOG(ERR,"CProtocolISO15765::IOCTL: pConfig==NULL!");
				return ERR_NULL_PARAMETER;
				}
			err=GetIOCTLParam(pConfig);
			if (err==ERR_NOT_SUPPORTED)
			{
				// reverting to base class handler
				err = ::GetIOCTLParam(pConfig);
			} 
			if (err!=STATUS_NOERROR)
			{
			LOG(ERR,"CProtocolISO15765::IOCTL: exiting !");
			return err;
			}
		}
		break;
		}
	case SET_CONFIG:
		{
		if (pInput==NULL)
			{
			LOG(ERR,"CProtocolCAN::IOCTL: pInput==NULL!");
			return ERR_NULL_PARAMETER;
			}
		SCONFIG_LIST * pList = (SCONFIG_LIST*)pInput;
		for (unsigned int i=0;i<pList->NumOfParams;i++)
			{
			SCONFIG * pConfig = &pList->ConfigPtr[i];
			if (pConfig==NULL)
				{
				LOG(ERR,"CProtocolCAN::IOCTL: pConfig==NULL!");
				return ERR_NULL_PARAMETER;
				}
			err=SetIOCTLParam(pConfig);
			if (err==ERR_NOT_SUPPORTED)
			{
				// reverting to base class handler
				err = ::SetIOCTLParam(pConfig);
			} 
			if (err!=STATUS_NOERROR)
				{
				LOG(ERR,"CProtocolCAN::IOCTL: exiting !");
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
		LOG(MAINFUNC,"CProtocolCAN::IOCTL----- NOT SUPPORTED -----");
		break;

	}

	return STATUS_NOERROR;
}
*/

int CProtocolISO15765::GetFilterById( unsigned long FilterId )
{
	unsigned int i=0;
	while (i<filtersnum)
	{
		if (filters[i].id==FilterId)
			return i;
		i++;
	}
	return -1;
}

int CProtocolISO15765::CreateFilter( PASSTHRU_MSG * pMaskMsg, PASSTHRU_MSG * pPatternMsg, PASSTHRU_MSG * pFlowControlMsg, unsigned long passFilterId )
{
	int len=4;
	if (pFlowControlMsg->TxFlags & ISO15765_EXT_ADDR)
	{
		// we use extended addressing, so we copy the PCI byte as well
		len++;
		filters[filtersnum].ext_addr=true;
	} else
		filters[filtersnum].ext_addr=false;

	int i;
	for (i=0;i<len;i++)
		{
			filters[filtersnum].mask[i] = pMaskMsg->Data[i];
			filters[filtersnum].pattern[i] = pPatternMsg->Data[i];
			filters[filtersnum].flowControlMsg[i] = pFlowControlMsg->Data[i];
		}
	if (pFlowControlMsg->TxFlags & ISO15765_FRAME_PAD)
	{
		// we pad the rest with zeros
		if  (len<12)
		{
			len = 12; // maximum CAN frame size: 4 header bytes, 8 data bytes
			for (;i<len;i++)
			{
				filters[filtersnum].mask[i] = 0;
				filters[filtersnum].pattern[i] = 0;
				filters[filtersnum].flowControlMsg[i] = 0;
			}
		}
	}

	// increment the filter id counter and set it as a new id
	filters[filtersnum].id = _running_filter_id++;
	filters[filtersnum].len = len;
	filters[filtersnum].passFilterId = passFilterId;
	filtersnum++;
	return filtersnum-1;	// return the new filter index #
}


int CProtocolISO15765::GetMatchingFilter( PASSTHRU_MSG * pMsg, bool compareToflowControlMsg )
{
	LOGW(PROTOCOL_VERBOSE,_T("CProtocolISO15765::GetMatchingFilter - compareToflowControlMsg: %d"), compareToflowControlMsg);
	unsigned int i=0;
	while (i<filtersnum)
	{
		int len = 4;
		// in case of extended addressing, we compare the first 5 bytes, otherwise just the arbitration id (4 bytes)
		if (filters[i].ext_addr)
			len++;

		// both filter and message must use the same type of addressing (normal or extended) to match
		bool msg_ext_addressing = (pMsg->TxFlags & ISO15765_EXT_ADDR) ? 1 : 0;
		if (msg_ext_addressing != filters[i].ext_addr)
		{
			LOGW(PROTOCOL_VERBOSE,_T("CProtocolISO15765::GetMatchingFilter - mismatch in addressing - filter:%d, msg: %d"), filters[i].ext_addr,msg_ext_addressing);
			return false;
		}

		int matching_bytes=0;
		for (int j=0;j<len;j++)
		{
			unsigned char masked = pMsg->Data[j] & filters[i].mask[j];
			if (compareToflowControlMsg)
				matching_bytes += ((masked==filters[i].flowControlMsg[j]) ? 1 : 0);
			else
				matching_bytes += ((masked==filters[i].pattern[j]) ? 1 : 0);
		}

		if (matching_bytes==len)
		{
			LOGW(PROTOCOL_VERBOSE,_T("CProtocolISO15765::GetMatchingFilter - matched filter #%d !"), i);
			return true;
		}
			
		i++;
	}
	// no match found
	LOGW(PROTOCOL_VERBOSE,_T("CProtocolISO15765::GetMatchingFilter - no match found"));
	return -1;
}

	
bool CProtocolISO15765::DoesMatchFilter( PASSTHRU_MSG * pMsg,  bool compareToFlowMsg )
{
	if (GetMatchingFilter(pMsg,compareToFlowMsg)==-1)
		return false;
	return true;
}


int CProtocolISO15765::StartMsgFilter( unsigned long FilterType, PASSTHRU_MSG * pMaskMsg, PASSTHRU_MSG * pPatternMsg, PASSTHRU_MSG * pFlowControlMsg, unsigned long * pFilterID )
{
	LOGW(PROTOCOL,_T("CProtocolISO15765::StartMsgFilter - filtertype %s"), dbug_filter2str(FilterType));
	if (FilterType!=FLOW_CONTROL_FILTER)
	{
		// pass to lower level impl
		return CProtocol::StartMsgFilter(FilterType,pMaskMsg,pPatternMsg,pFlowControlMsg,pFilterID);
	}

	dbug_printmsg(pFlowControlMsg,_T("FlowControlMsg"),1,true);

	// pPatternMsg & pMaskMsg will be checked below in CProtocol implementation
	if (pFlowControlMsg->ProtocolID!= ProtocolID())
	{
		LOG(ERR,"CProtocolISO15765::StartMsgFilter: invalid protocol ID %d in MaskMsg",pFlowControlMsg->ProtocolID);
		return ERR_MSG_PROTOCOL_ID;
	}
	if (pFlowControlMsg->DataSize > 12)
	{
		LOG(ERR,"CProtocolISO15765::StartMsgFilter: data length %d > 12 in pFlowControlMsg",pFlowControlMsg->DataSize);
		return ERR_INVALID_MSG;
	}
	if (DoesMatchFilter(pPatternMsg,true) || DoesMatchFilter(pPatternMsg,false))
	{
		LOG(ERR,"CProtocolISO15765::StartMsgFilter: pPatternMsg matches existing flow control filter!");
		return ERR_NOT_UNIQUE;
	}
	if (DoesMatchFilter(pFlowControlMsg,true) || DoesMatchFilter(pFlowControlMsg,false))
	{
		LOG(ERR,"CProtocolISO15765::StartMsgFilter: pFlowControlMsg matches existing flow control filter!");
		return ERR_NOT_UNIQUE;
	}

	// create corresponding pass filter
	unsigned long passFilterID;
	int err=CProtocol::StartMsgFilter(PASS_FILTER,pMaskMsg,pPatternMsg,NULL,&passFilterID);
	if (err!=STATUS_NOERROR)
		return err;

	// ok, let's create a new filter
	int idx = CreateFilter(pMaskMsg,pPatternMsg,pFlowControlMsg,passFilterID);
	*pFilterID = filters[idx].id;

	return STATUS_NOERROR;
}


int CProtocolISO15765::StopMsgFilter(  unsigned long FilterID )
{
	LOG(PROTOCOL,"CProtocolISO15765::StopMsgFilter - filter id %d", FilterID);
#ifdef PLAY_STUPID
	LOG(ERR,"CProtocolISO15765::StopMsgFilter - not yet implemented, ignored");
	return STATUS_NOERROR;
#else
	LOG(ERR,"CProtocolISO15765::StopMsgFilter  --- FIXME ! does not exist");
	return ERR_NOT_SUPPORTED;
#endif
}