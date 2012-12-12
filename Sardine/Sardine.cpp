// Sardine CAN : (C) Olaf @ Hackingvolvo blog (hackingvolvo@blogspot.com)
// Sardine.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Sardine.h"
#include "sardine_defs.h"
#include "comm.h"
#include "helper.h"
#include "Benaphore.h"
#include "ProtocolCAN.h"
#include "ProtocolISO9141.h"
#include "ProtocolJ1850VPW.h"
#include "ProtocolISO15765.h"


namespace Sardine {

#define MAX_CHANNELS 8

	Benaphore channel_lock;

typedef struct {
	int channelId;
	int protocolId;
	CProtocol * handler;
} channel;

	channel * channels[MAX_CHANNELS];
	int channelcount=0;			// how many active channels

	int channel_id_counter=0;	// counter for unique channels (only increments). We don't recycle channel ids just to be safe.

	char last_error_msg[80];	// valid if last_error==ERR_FAILED (in order for user to get more specified error message via PassThruGetLastError)
	int last_error = 0;

channel * GetChannelByChannelId( int channel_id )
{
	int i=0;
	channel_lock.Lock();
	while (i<channelcount)
	{
		if (channels[i]->channelId==channel_id)
		{
			channel * c = channels[i];
			channel_lock.Unlock();
			return c;
		}
		i++;
	}
	channel_lock.Unlock();
	LOG(ERR,"GetChannelByChannelId: no such channel id! %d",channel_id);
	return NULL;
}

channel * GetchannelByProtocolId( int protocol_id )
{
	int i=0;
	channel_lock.Lock();
	while (i<channelcount)
	{
		if (channels[i]->protocolId==protocol_id)
		{
			channel * c = channels[i];
			channel_lock.Unlock();
			return c;
		}
		i++;
	}
	channel_lock.Unlock();
	LOG(HELPERFUNC,"GetchannelByProtocolId: no channel associated with protocol %d",protocol_id);
	return NULL;
}

int DeleteChannel( int channel_id )
	{
	LOG(HELPERFUNC,"DeleteChannel");
	channel_lock.Lock();
	int i=0;
	while ( (i<channelcount) && (channels[i]->channelId!=channel_id) )
		i++;
	if (i==channelcount)
		{
		LOG(ERR,"DeleteChannel: invalid channel id! %d",channel_id);
		channel_lock.Unlock();
		return -1;
		}
	if (channels[i]->handler)
		delete channels[i]->handler;
	delete channels[i];
	while (i<channelcount-1)
	{
		channels[i] = channels[i+1];
		i++;
	}
	channelcount--;
	channel_lock.Unlock();
	return 0;
	}


int AddChannel( channel * c )
	{
	LOG(HELPERFUNC,"AddChannel");
	channel_lock.Lock();
	if (channelcount<MAX_CHANNELS)
		{
		channels[channelcount++] = c;
		}
	else
		{
		LOG(ERR,"AddChannel: Too many channels!");
		channel_lock.Unlock();
		return -1;
		}
	channel_lock.Unlock();
	return 0;
	}


void Close()
{
	LOG(MAINFUNC,"Sardine::Close");
	channel_lock.Lock();
	for (int i=0;i<channelcount;i++)
		{
			channels[i]->handler->Disconnect();
			delete channels[i]->handler;
		}

	channel_lock.Unlock();
	LOG(MAINFUNC,"Sardine::Closing down completed");
}

///////////////////////////////////// PassThruFunctions /////////////////////////////////////////////////

DllExport PassThruOpen(void *pName, unsigned long *pDeviceID)
{
	if (pName!=NULL)
		{
		LOG(MAINFUNC,"PassThruOpen: pName [%s]",pName);
		}
	else
		{
		LOG(MAINFUNC,"PassThruOpen: pName==NULL");
		}

	*pDeviceID = 0xdeadbeef;
	return last_error=STATUS_NOERROR;
}

DllExport PassThruClose(unsigned long DeviceID)
{
	LOG(MAINFUNC,"PassThruClose: DeviceID 0x%x",DeviceID);
	return last_error=STATUS_NOERROR;
}


DllExport PassThruConnect(unsigned long DeviceID, unsigned long ProtocolID, unsigned long Flags, unsigned long Baudrate, unsigned long *pChannelID)
{
	LOG(MAINFUNC,"\nPassThruConnect: device id: 0x%x, protocol Id 0x%x, flags: 0x%x, baudrate: %d",DeviceID,ProtocolID,Flags,Baudrate);
	int err;

	channel * ch = GetchannelByProtocolId(ProtocolID);
	if (ch != NULL)
		{
		// According to specs, there can be only one channel by protocol in use. 
		// (I might change this to allow read-only access for additional debugging/logging
		LOG(ERR,"PassThruConnect: Channel already in use!");
		return last_error=ERR_CHANNEL_IN_USE;
		}
	
	if (pChannelID==NULL)
		return last_error=ERR_NULL_PARAMETER;

	if (!Comm::WaitUntilInitialized(2000))
		{
		LOG(ERR,"PassThruConnect: Arduino wasn't initialized!");
		return last_error=ERR_DEVICE_NOT_CONNECTED;
		}

	ch = new channel();
	if (ch==NULL)
		{
		LOG(ERR,"PassThruConnect: Out of memory!");
		strcpy_s(last_error_msg,80,"Out of memory!");
		return ERR_FAILED;
		}
	ch->protocolId = ProtocolID;
	ch->handler = NULL;

	if ( (ProtocolID==ISO9141) || (ProtocolID==ISO9141_PS) ) //PROTOCOL_ID_ISO9141)
		{
		LOG(PROTOCOL,"PassThruConnect: ISO9141");
		ch->handler = new CProtocolISO9141(ProtocolID);
		}/*
	else if (ProtocolID==PROTOCOL_ID_ISO14230)
		{
		LOG("PassThruConnect: ISO 14230: ");
		channels[curr_channel].handler = new CProtocolISO9141();
		if (channels[curr_channel].CAN29bit)
			LOG(" 29bit addressing")
		else 
			LOG(" 11bit addressing");
		if (channels[curr_channel].extaddr)
			LOG(" extended addressing")
		else 
			LOG(" standard addressing");
		}*/
	else if ( (ProtocolID==CAN) || (ProtocolID==CAN_PS) )//PROTOCOL_ID_RAW_CAN)
		{
		LOG(PROTOCOL,"PassThruConnect: CAN ");
		ch->handler = new CProtocolCAN(ProtocolID);
		}
	else if ( (ProtocolID==J1850VPW) || (ProtocolID==J1850VPW_PS) ) //PROTOCOL_ID_RAW_CAN)
		{
		LOG(PROTOCOL,"PassThruConnect: J1850VPW ");
		ch->handler = new CProtocolJ1850VPW(ProtocolID);
		}
	else if ( (ProtocolID==ISO15765) || (ProtocolID==ISO15765_PS) )
		{
		LOG(PROTOCOL,"PassThruConnect: ISO 15765");
		ch->handler = new CProtocolISO15765(ProtocolID);	
		}
	else 
		{
#ifdef PLAY_STUPID_WITH_VIDA
		ch->handler = new CProtocolCAN(ProtocolID);
		LOG(ERR,"PassThruConnect: FIXME Protocol not supported! -> defaulting to CAN");
#else
		LOG(ERR,"PassThruConnect: Protocol not supported! ");
		delete ch;
		return last_error=ERR_NOT_SUPPORTED;
#endif
		}

	// let's increment unique channel id counter
	channel_id_counter++;
	ch->channelId = channel_id_counter;
	*pChannelID=ch->channelId;

	// initialize the handler
	if ((err=ch->handler->Connect(ch->channelId,Flags)) != STATUS_NOERROR)
		{
		LOG(ERR,"PassThruConnect: Protocol handler init failed! ");
		delete ch;
		return err;
		}


	if (AddChannel(ch)==-1)	// takes ownership
		{
		LOG(ERR,"PassThruConnect: Too many channels open! ");
		strcpy_s(last_error_msg,80,"Too many channels open!");
		delete ch;
		return ERR_FAILED;
		}

	LOG(MAINFUNC,"PassThruConnect: Assinging channel id %d",ch->channelId);
	return last_error=STATUS_NOERROR;
}


DllExport PassThruDisconnect(unsigned long ChannelID)
{
	LOG(MAINFUNC,"PassThruDisconnect: channel Id %d",ChannelID);

	channel * ch = GetChannelByChannelId(ChannelID);
	if (ch == NULL)
		{
		LOG(ERR,"PassThruConnect: Invalid channel id");
		return last_error=ERR_INVALID_CHANNEL_ID;
		}

	ch->handler->Disconnect();
	DeleteChannel(ChannelID);
	return last_error=STATUS_NOERROR;
}


DllExport PassThruReadMsgs(unsigned long ChannelID, PASSTHRU_MSG *pMsg, unsigned long *pNumMsgs, unsigned long Timeout)
{
	LOG(MAINFUNC,"PassThruReadMsgs: channel Id %d, timeout: %d",ChannelID,Timeout);

	if (pMsg==NULL)
	{
		LOG(ERR,"PassThruReadMsgs: Null parameter!");
		return ERR_NULL_PARAMETER;
	}
	if (pNumMsgs==NULL)
	{
		LOG(ERR,"PassThruReadMsgs: Null parameter!");
		return ERR_NULL_PARAMETER;
	}

	channel * ch = GetChannelByChannelId(ChannelID);
	if (ch == NULL)
		{
		LOG(ERR,"PassThruReadMsgs: Invalid channel id");
		return last_error=ERR_INVALID_CHANNEL_ID;
		}

	return last_error=ch->handler->ReadMsgs(pMsg,pNumMsgs,Timeout);
}
		

DllExport PassThruWriteMsgs(unsigned long ChannelID, PASSTHRU_MSG *pMsg, unsigned long *pNumMsgs, unsigned long Timeout)
{
	LOG(MAINFUNC,"PassThruWriteMsgs: channel Id %d, timeout: %d",ChannelID,Timeout);

	if (pMsg==NULL)
	{
		LOG(ERR,"PassThruWriteMsgs: Null parameter!");
		return ERR_NULL_PARAMETER;
	}
	if (pNumMsgs==NULL)
	{
		LOG(ERR,"PassThruWriteMsgs: Null parameter!");
		return ERR_NULL_PARAMETER;
	}

	channel * ch = GetChannelByChannelId(ChannelID);
	if (ch == NULL)
		{
		LOG(ERR,"PassThruWriteMsgs: Invalid channel id");
		return last_error=ERR_INVALID_CHANNEL_ID;
		}	
	
	return last_error=ch->handler->WriteMsgs(pMsg,pNumMsgs,Timeout);
}


DllExport PassThruStartPeriodicMsg(unsigned long ChannelID, PASSTHRU_MSG *pMsg, unsigned long *pMsgID, unsigned long TimeInterval)
{
	LOG(MAINFUNC,"PassThruStartPeriodicMsg: channel Id %d, time interval: %d",ChannelID,TimeInterval);
	
	if (pMsg==NULL)
	{
		LOG(ERR,"PassThruStartPeriodicMsg: Null parameter!");
		return ERR_NULL_PARAMETER;
	}
	if (pMsgID==NULL)
	{
		LOG(ERR,"PassThruStartPeriodicMsg: Null parameter!");
		return ERR_NULL_PARAMETER;
	}

	channel * ch = GetChannelByChannelId(ChannelID);
	if (ch == NULL)
		{
		LOG(ERR,"PassThruStartPeriodicMsg: Invalid channel id");
		return last_error=ERR_INVALID_CHANNEL_ID;
		}

	return last_error=ch->handler->StartPeriodicMsg(pMsg,pMsgID,TimeInterval);
}


DllExport PassThruStopPeriodicMsg(unsigned long ChannelID, unsigned long MsgID)
{
	LOG(MAINFUNC,"PassThruStopPeriodicMsg: channel Id %d, msgID: 0x%x",ChannelID,MsgID);
	channel * ch = GetChannelByChannelId(ChannelID);
	if (ch == NULL)
		{
		LOG(ERR,"PassThruStopPeriodicMsg: Invalid channel id");
		return last_error=ERR_INVALID_CHANNEL_ID;
		}

	return last_error=ch->handler->StopPeriodicMsg(MsgID);
}


DllExport PassThruStartMsgFilter(unsigned long ChannelID, unsigned long FilterType, PASSTHRU_MSG *pMaskMsg, PASSTHRU_MSG *pPatternMsg, PASSTHRU_MSG *pFlowControlMsg, unsigned long *pFilterID)
{
	LOG(MAINFUNC,"PassThruStartMsgFilter: channel Id %d, filter type: %d",ChannelID,FilterType);

	if (pMaskMsg==NULL)
	{
		LOG(ERR,"PassThruStartMsgFilter: pMaskMsg==NULL !");
		return ERR_NULL_PARAMETER;
	}
	if (pPatternMsg==NULL)
	{
		LOG(ERR,"PassThruStartMsgFilter: pPatternMsg==NULL !");
		return ERR_NULL_PARAMETER;
	}
	if ( (FilterType==FLOW_CONTROL_FILTER) && (pFlowControlMsg==NULL) )
	{
		LOG(ERR,"PassThruStartMsgFilter: pFlowControlMsg==NULL !");
		return ERR_NULL_PARAMETER;
	}	
	if (pFilterID==NULL)
	{
		LOG(ERR,"PassThruStartMsgFilter: pFilterID==NULL !");
		return ERR_NULL_PARAMETER;
	}

	channel * ch = GetChannelByChannelId(ChannelID);
	if (ch == NULL)
		{
		LOG(ERR,"PassThruStartMsgFilter: Invalid channel id");
		return last_error=ERR_INVALID_CHANNEL_ID;
		}

	return last_error=ch->handler->StartMsgFilter(FilterType,pMaskMsg,pPatternMsg,pFlowControlMsg,pFilterID);
}


DllExport PassThruStopMsgFilter(unsigned long ChannelID, unsigned long FilterID)
{
	LOG(MAINFUNC,"PassThruStopMsgFilter: channel Id %d, filter ID: 0x%x",ChannelID,FilterID);
	channel * ch = GetChannelByChannelId(ChannelID);
	if (ch == NULL)
		{
		LOG(ERR,"PassThruStopMsgFilter: Invalid channel id");
		return last_error=ERR_INVALID_CHANNEL_ID;
		}

	return last_error=ch->handler->StopMsgFilter(FilterID);
}


DllExport PassThruSetProgrammingVoltage(unsigned long DeviceID, unsigned long PinNumber, unsigned long Voltage)
{
	LOG(MAINFUNC,"PassThruSetProgrammingVoltage: device id: 0x%x, pin number %d, voltage: %d",DeviceID,PinNumber,Voltage);

#ifdef PLAY_STUPID
	LOG(ERR,"PassThruSetProgrammingVoltage:----- NOT SUPPORTED -----, ignoring");
	return STATUS_NOERROR;
#else
	LOG(ERR,"PassThruSetProgrammingVoltage:----- NOT SUPPORTED -----");
	return last_error=ERR_NOT_SUPPORTED;
#endif
}


DllExport PassThruReadVersion(unsigned long DeviceID, char *pFirmwareVersion, char *pDllVersion, char *pApiVersion)
{
	LOG(MAINFUNC,"PassThruReadVersion: deviceId 0x%x",DeviceID);
	if (pFirmwareVersion==NULL)
		{
		return last_error=ERR_NULL_PARAMETER;
		}
	if (pDllVersion==NULL)
		{
		return last_error=ERR_NULL_PARAMETER;
		}
	if (pApiVersion==NULL)
		{
		return last_error=ERR_NULL_PARAMETER;
		}

	// according to specs, destination buffers must be atleast 80 chars long
	strcpy_s(pFirmwareVersion,80,"01.00");
	strcpy_s(pDllVersion,80,"00.01");
	strcpy_s(pApiVersion,80,"04.04");
	return last_error=STATUS_NOERROR;
}


// We don't alter any last_errors in this function
DllExport PassThruGetLastError(char *pErrorDescription)
{
	LOG(MAINFUNC,"PassThruGetLastError: (is %d)",last_error);
	if (pErrorDescription==NULL)
	{
		LOG(ERR,"PassThruGetLastError: Null parameter!");
		return ERR_NULL_PARAMETER;
	}

	if (last_error==ERR_FAILED)
		{
		// according to specs, destination buffer must be at least 80 chars long
		LOG(MAINFUNC,"PassThruGetLastError: [%s]",last_error_msg);
		strcpy_s(pErrorDescription,80,last_error_msg);
		return STATUS_NOERROR;
		}
	else
	{
		int i=0;
		while (J2534_error_msgs[i].msg!=NULL) {
			if (last_error==J2534_error_msgs[i].err_id)
				{
				// according to specs, destination buffer must be at least 80 chars long
				LOG(MAINFUNC,"PassThruGetLastError:  [%s]",J2534_error_msgs[i].msg);
				strcpy_s(pErrorDescription,80,J2534_error_msgs[i].msg);
				return STATUS_NOERROR;
				}
			i++;
		}
	}
	// should not end here
	LOG(ERR,"PassThruGetLastError: Invalid error value! (%d)",last_error);
	// according to specs, destination buffer must be at least 80 chars long
	strcpy_s(last_error_msg,80,"Internal Sardine Error");
	return ERR_FAILED;
}


DllExport PassThruIoctl(unsigned long ChannelID, unsigned long IoctlID, void *pInput, void *pOutput)
{
	LOG(MAINFUNC,"PassThruIoctl: channel Id %d, ioctl id: %d",ChannelID,IoctlID);
	Print_IOCtl_Cmd( IoctlID );

	channel * ch = GetChannelByChannelId(ChannelID);
	if (ch == NULL)
		{
		LOG(ERR,"PassThruIoctl: Invalid channel id");
		return last_error=ERR_INVALID_CHANNEL_ID;
		}

	// check and debug IOCTL commands, parameters and values. Handle here the general commands (non-protocol spesific)
	switch (IoctlID)
	{
	case GET_CONFIG:
		// handled by protocol impl
		break;
	case SET_CONFIG:
		// handled by protocol impl
		Print_SConfig_List((SCONFIG_LIST *) pInput);
		break;
	case READ_VBATT:
		LOG(ERR,"PassThruIoctl:----- NOT SUPPORTED (returning default 12 volts)-----");
		*(unsigned long*)pOutput=12*1000;	// 12000 millivolts  FIXME
		return STATUS_NOERROR;
		break;
	case FIVE_BAUD_INIT:
		Print_SByte_Array((SBYTE_ARRAY *) pInput);
		LOG(ERR,"PassThruIoctl:----- NOT SUPPORTED -----");
		break;
	case FAST_INIT:
//		dbug_printmsg((PASSTHRU_MSG *) pInput, _T("Input"), 1, true);
		LOG(ERR,"PassThruIoctl:----- NOT SUPPORTED -----");
		break;
	case CLEAR_TX_BUFFER:
		// handled by protocol impl
		break;
	case CLEAR_RX_BUFFER:
		// handled by protocol impl
		break;
	case CLEAR_PERIODIC_MSGS:
		// handled by protocol impl
		break;
	case CLEAR_MSG_FILTERS:
		LOG(ERR,"PassThruIoctl:----- NOT SUPPORTED -----");
		break;
	case CLEAR_FUNCT_MSG_LOOKUP_TABLE:
		LOG(ERR,"PassThruIoctl:----- NOT SUPPORTED -----");
		break;
	case ADD_TO_FUNCT_MSG_LOOKUP_TABLE:
		Print_SByte_Array((SBYTE_ARRAY *) pInput);
		LOG(ERR,"PassThruIoctl:----- NOT SUPPORTED -----");
		break;
	case DELETE_FROM_FUNCT_MSG_LOOKUP_TABLE:
		Print_SByte_Array((SBYTE_ARRAY *) pInput);
		LOG(ERR,"PassThruIoctl:----- NOT SUPPORTED -----");
		break;
	case READ_PROG_VOLTAGE:
		LOG(ERR,"PassThruIoctl:----- NOT SUPPORTED -----");
		break;
	default:
		LOG(ERR,"PassThruIoctl: Invalid IOCTL command!");
		return ERR_INVALID_IOCTL_ID;
		break;

	}

	return last_error=ch->handler->IOCTL(IoctlID,pInput,pOutput);
}


}