#include "stdafx.h"
#include "helper.h"
#include "../Sardine/sardinedll.h"
#include "../sardine/sardine_defs.h"

int ConnectTests()
	{
	int failed=0;
	unsigned long int channelId;
	unsigned long ret;

	LOG("ConnectTest +1");
	ret=PassThruConnect(SARDINE_DEVICE_ID, CAN,1<<8,SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (!ret)
	{
		ret=PassThruDisconnect(channelId);
		ParseReturnValue(ret);
		if (ret!=STATUS_NOERROR)
			failed++;
	} else failed++;

	LOG("ConnectTest +2");
	ret=PassThruConnect(SARDINE_DEVICE_ID,ISO15765,1<<8,SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (!ret)
	{
		ret=PassThruDisconnect(channelId);
		ParseReturnValue(ret);
		if (ret!=STATUS_NOERROR)
			failed++;
	} else failed++;

	LOG("ConnectTest +3");
	ret=PassThruConnect(SARDINE_DEVICE_ID,CAN,(1<<8)+(1<<7),SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (!ret)
	{
		ret=PassThruDisconnect(channelId);
		ParseReturnValue(ret);
		if (ret!=STATUS_NOERROR)
			failed++;
	} else failed++;

	LOG("ConnectTest +4");
	ret=PassThruConnect(SARDINE_DEVICE_ID,ISO15765,(1<<8)+(1<<7),SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (!ret)
	{
		ret=PassThruDisconnect(channelId);
		ParseReturnValue(ret);
		if (ret!=STATUS_NOERROR)
			failed++;
	} else failed++;

	LOG("ConnectTest +5");
	ret=PassThruConnect(SARDINE_DEVICE_ID,ISO9141,1<<8,SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (!ret)
	{
		ret=PassThruDisconnect(channelId);
		ParseReturnValue(ret);
		if (ret!=STATUS_NOERROR)
			failed++;
	} else failed++;

	LOG("ConnectTest +6");
	ret=PassThruConnect(SARDINE_DEVICE_ID,ISO14230,1<<8,SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (!ret)
	{
		ret=PassThruDisconnect(channelId);
		ParseReturnValue(ret);
		if (ret!=STATUS_NOERROR)
			failed++;
	} else failed++;

	// incorrect channel ids
	LOG("ConnectTest -1");
	channelId=-1;
	ret=PassThruDisconnect(channelId);
	if (ret != ERR_INVALID_CHANNEL_ID)
		{
		ParseReturnValue(ret);
		failed++;
		}

	LOG("ConnectTest -2");
	channelId=0;
	ret=PassThruDisconnect(channelId);
	if (ret != ERR_INVALID_CHANNEL_ID)
		{
		ParseReturnValue(ret);
		failed++;
		}

	LOG("ConnectTest -3");
	channelId=1;
	ret=PassThruDisconnect(channelId);
	if (ret != ERR_INVALID_CHANNEL_ID)
		{
		ParseReturnValue(ret);
		failed++;
		}

	return failed;
	}


int ReadTests()
	{
		/*
	typedef struct {
	unsigned long ProtocolID; // vehicle network protocol 
	unsigned long RxStatus; // receive message status 
	unsigned long TxFlags; // transmit message flags 
	unsigned long Timestamp; // receive message timestamp(in microseconds) 
	unsigned long DataSize; // byte size of message payload in the Data array 
	unsigned long ExtraDataIndex; // start of extra data(i.e. CRC, checksum, etc) in Data array 
	unsigned char Data[4128]; // message payload or data 
	} PASSTHRU_MSG;
	*/

	unsigned long ret;
	unsigned long channelId; /* Logical channel identifier returned by PassThruConnect */
	unsigned long NumMsgs;
	PASSTHRU_MSG Msg[2];
	unsigned long Timeout;
	char errstr[256];
	int failed=0;

	/*
	** Establish a ISO15765 communication channel to the vehicle network.
	*/
	LOG("ReadTest 1:1");
	ret = PassThruConnect(SARDINE_DEVICE_ID,ISO15765, 0x00000000, SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (!ret)
	{
		/*
		** Initialize the PASSTHRU_MSG structure to all zeroes.
		** Set the ProtocolID to select protocol frames of interest.
		** The API/DLL will fill in RxStatus, TxStaus, Timestamp, DataSize, ExtraDataIndex and Data
		** after the function call completes.
		*/
		memset(&Msg, 0, sizeof(Msg));

		Msg[0].ProtocolID = ISO15765;
		Msg[1].ProtocolID = ISO15765;

		/*
		** Indicate that PASSTHRU_MSG array contains two messages.
		*/
		NumMsgs = 2;

		/*
		** API/DLL should read first two messages in receive queue and immediately return. If there aren’t any
		** messages in the receive queue then API/DLL will return ERR_BUFFER_EMPTY.
		*/
		Timeout = 0;
		LOG("ReadTest 1:2");
		ret = PassThruReadMsgs(channelId, &Msg[0], &NumMsgs, Timeout);
		ParseReturnValue(ret);
		if (ret != STATUS_NOERROR)
		{
			/*
			** PassThruReadMsgs failed! Get descriptive error string.
			*/
			PassThruGetLastError(&errstr[0]);
			LOG("ReadMsgs failed: %s",errstr);
			failed++;
		}
		ret=PassThruDisconnect(channelId);
		ParseReturnValue(ret);
		if (ret != STATUS_NOERROR)
			failed++;
	} else failed++;

	/*
	** Establish a RAW CAN communication channel to the vehicle network.
	*/
	LOG("ReadTest 2:1");
	ret = PassThruConnect(SARDINE_DEVICE_ID,CAN, 0x00000000, SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (!ret)
	{
		/*
		** Initialize the PASSTHRU_MSG structure to all zeroes.
		** Set the ProtocolID to select protocol frames of interest.
		** The API/DLL will fill in RxStatus, TxStaus, Timestamp, DataSize, ExtraDataIndex and Data
		** after the function call completes.
		*/
		memset(&Msg, 0, sizeof(Msg));

		Msg[0].ProtocolID = CAN;
		Msg[1].ProtocolID = CAN;

		/*
		** Indicate that PASSTHRU_MSG array contains two messages.
		*/
		NumMsgs = 2;

		/*
		** API/DLL should read first two messages in receive queue and immediately return. If there aren’t any
		** messages in the receive queue then API/DLL will return ERR_BUFFER_EMPTY.
		*/
		Timeout = 0;
		LOG("ReadTest 2:2");
		ret = PassThruReadMsgs(channelId, &Msg[0], &NumMsgs, Timeout);
		if (ret != STATUS_NOERROR)
		{
			/*
			** PassThruReadMsgs failed! Get descriptive error string.
			*/
			PassThruGetLastError(&errstr[0]);
			LOG("ReadMsgs failed: %s",errstr);
		}

		// second read, but wait two seconds for msgs
		Timeout = 2000;
		LOG("ReadTest 3:2");
		ret = PassThruReadMsgs(channelId, &Msg[0], &NumMsgs, Timeout);
		if (ret != STATUS_NOERROR)
		{
			/*
			** PassThruReadMsgs failed! Get descriptive error string.
			*/
			PassThruGetLastError(&errstr[0]);
			LOG("ReadMsgs failed: %s",errstr);
			failed++;
		}
		ret=PassThruDisconnect(channelId);
		ParseReturnValue(ret);
		if (ret != STATUS_NOERROR)
			failed++;
	} else failed++;
	return failed;
}


int WriteTests()
	{
	PASSTHRU_MSG MaskMsg;
	PASSTHRU_MSG PatternMsg;
	PASSTHRU_MSG FilterMsg;
	SCONFIG CfgItem[2];
	SCONFIG_LIST Input;
	int failed =0;
	unsigned long status;
	unsigned long ChannelID; /* Logical channel identifier returned by PassThruConnect */
	unsigned long FilterID;

	unsigned long ret;
	unsigned long NumMsgs;
	PASSTHRU_MSG Msg[1];
	unsigned long Timeout;
	char errstr[256];

	/*
	** Establish a ISO15765 communication channel to the vehicle network.
	*/
	LOG("WriteTest 1:1");
	ret = PassThruConnect(SARDINE_DEVICE_ID,ISO15765, 0x00000000, SARDINE_DEFAULT_CAN_BAUD_RATE, &ChannelID);
	ParseReturnValue(ret);
	if (!ret)
	{

		/*
		** Setup the PassThru device receive message assembly capabilities.
		** BlockSize is the number ConsecutiveFrames that can be received in a burst.
		** SeparationTime is the minimum time to pause between transmitting ConsecutiveFrames.
		*/
		CfgItem[0].Parameter = ISO15765_BS;
		CfgItem[0].Value = 0x20; /* BlockSize is 32 frames */
		CfgItem[1].Parameter = ISO15765_STMIN;
		CfgItem[1].Value = 0x01; /* SeparationTime is 1 millisecond */
		Input.NumOfParams = 2; /* Configuration list has 2 items */
		Input.ConfigPtr = &CfgItem[0];
		/*

		** The PassThru device is asked to use the UserApplication selected BlockSize and
		** SeparationTime values instead of the J2534 specified defaults.
		*/
		status = PassThruIoctl(ChannelID, SET_CONFIG, (void *)&Input, NULL);
		/*
		** Set RxStaus, TxFlags, TimeStamp and ExtraDataIndex to zero.
		** TxFlags = 0x00000000 implies:
		** No blocking on Tx complete, 11-bit CAN Identifier, normal addressing,
		** no zero padding of FlowControl message.
		** The ProtocolID, DataSize and Data array are set below.
		*/
		memset(&MaskMsg, 0, sizeof(MaskMsg));
		/*
		** This example assumes 11-bit CAN Identifiers are used by the ECUs connected to the bus.
		** Set the Mask to look at all 11 bits of the Can Identifier portion of a receive frame.
		** MaskMsg.Data[0] and MaskMsg[0].Data[1] set to zero by previous memset.
		*/
		MaskMsg.ProtocolID = ISO15765;
		MaskMsg.Data[2] = 0x07;
		MaskMsg.Data[3] = 0xFF;
		MaskMsg.DataSize = 4; /* Mask message contains 4 bytes */


		/*
		** Set RxStaus, TxFlags, TimeStamp and ExtraDataIndex to zero.
		** TxFlags = 0x00000000 implies:
		** No blocking on Tx complete, 11-bit CAN Identifier, normal addressing,
		** no zero padding of FlowControl message.
		** The ProtocolID, DataSize and Data array are set below.
		*/
		memset(&PatternMsg, 0, sizeof(PatternMsg));
		/*
		** The Can Identifier is a 11-bit OBD CAN Identifier used for physically addressed response messages
		** originating from ECU #1 and destined to the PassThru device.
		** PatternMsg.Data[0] and PatternMsg[0].Data[1] set to zero by previous memset.
		*/
		PatternMsg.ProtocolID = ISO15765;
		PatternMsg.Data[2] = 0x07;
		PatternMsg.Data[3] = 0xE0;
		PatternMsg.DataSize = 4; /* Pattern message contains 4 bytes */
		/*
		** Set RxStaus, TxFlags, TimeStamp and ExtraDataIndex to zero.
		** TxFlags = 0x00000000 implies:
		** No blocking on Tx complete, 11-bit CAN Identifier, normal addressing,
		** no zero padding of FlowControl message.
		** The ProtocolID, DataSize and Data array are set below.
		*/
		memset(&FilterMsg, 0, sizeof(FilterMsg));

		/*
		** The Can Identifier is a 11-bit OBD CAN Identifier used for physically addressed request messages
		** originating from the PassThru device and destined to ECU #1.
		** FilterMsg.Data[0] and FilterMsg[0].Data[1] set to zero by previous memset.*/
		FilterMsg.ProtocolID = ISO15765;
		FilterMsg.Data[2] = 0x07;
		FilterMsg.Data[3] = 0xE8;
		FilterMsg.DataSize = 4; /* Filter message contains 4 bytes */
		status = PassThruStartMsgFilter(ChannelID, FLOW_CONTROL_FILTER, &MaskMsg, &PatternMsg,
			&FilterMsg, &FilterID);
		if (status != STATUS_NOERROR)
		{
			/*
			** PassThruStartMsgFilter failed! Get descriptive error string.
			*/
			PassThruGetLastError(&errstr[0]);
			/*
			** Display Error dialog box and/or write error description to Log file.
			*/
			failed++;
		}


		/*
		** Initialize the PASSTHRU_MSG structure to all zeroes.
		** Set the ProtocolID to select protocol frames of interest.
		** The API/DLL will fill in RxStatus, TxStaus, Timestamp, DataSize, ExtraDataIndex and Data
		** after the function call completes.
		*/
		memset(&Msg, 0, sizeof(Msg));

		Msg[0].ProtocolID = ISO15765;
		// header: ECU
		/*
		Msg[0].Data[0] = 0x00;
		Msg[0].Data[1] = 0x0f;
		Msg[0].Data[2] = 0xff;
		Msg[0].Data[3] = 0xfe;
		*/
		Msg[0].Data[2] = 0x07;
		Msg[0].Data[3] = 0xE8;

		// request heater status
		Msg[0].Data[4] = 0xcd;
		Msg[0].Data[5] = 0x40;
		Msg[0].Data[6] = 0xa6;
		Msg[0].Data[7] = 0x5f;
		Msg[0].Data[8] = 0x32;
		Msg[0].Data[9] = 0x01;
		Msg[0].Data[10] = 0x00;
		Msg[0].Data[11] = 0x00;
		Msg[0].DataSize=12;

		/*
		** Indicate that PASSTHRU_MSG array contains 1 message.
		*/
		NumMsgs = 1;

		Timeout = 0;
		LOG("WriteTest 1:2");
		ret = PassThruWriteMsgs(ChannelID, &Msg[0], &NumMsgs, Timeout);
		if (ret != STATUS_NOERROR)
		{
			/*
			** PassThruWriteMsgs failed! Get descriptive error string.
			*/
			PassThruGetLastError(&errstr[0]);
			LOG("WriteTest failed: %s",errstr);
			failed++;
		}
		ret=PassThruDisconnect(ChannelID);
		ParseReturnValue(ret);
		if (ret != STATUS_NOERROR)
			failed++;
	} else failed++;





	/*
	** Establish a RAW CAN communication channel to the vehicle network.
	*/
	LOG("WriteTest 2:1");
	ret = PassThruConnect(SARDINE_DEVICE_ID,CAN, 0x00000000,SARDINE_DEFAULT_CAN_BAUD_RATE, &ChannelID);
	ParseReturnValue(ret);
	if (!ret)
	{
		/*
		** Initialize the PASSTHRU_MSG structure to all zeroes.
		** Set the ProtocolID to select protocol frames of interest.
		** The API/DLL will fill in RxStatus, TxStaus, Timestamp, DataSize, ExtraDataIndex and Data
		** after the function call completes.
		*/
		memset(&Msg, 0, sizeof(Msg));

		Msg[0].ProtocolID = CAN;
		// header: ECU
		Msg[0].Data[0] = 0x00;
		Msg[0].Data[1] = 0x0f;
		Msg[0].Data[2] = 0xff;
		Msg[0].Data[3] = 0xfe;
		// request heater status
		Msg[0].Data[4] = 0xcd;
		Msg[0].Data[5] = 0x40;
		Msg[0].Data[6] = 0xa6;
		Msg[0].Data[7] = 0x5f;
		Msg[0].Data[8] = 0x32;
		Msg[0].Data[9] = 0x01;
		Msg[0].Data[10] = 0x00;
		Msg[0].Data[11] = 0x00;
		Msg[0].DataSize=12;

		/*
		** Indicate that PASSTHRU_MSG array contains 1 message.
		*/
		NumMsgs = 1;

		Timeout = 0;
		LOG("WriteTest 2:2");
		ret = PassThruWriteMsgs(ChannelID, &Msg[0], &NumMsgs, Timeout);
		if (ret != STATUS_NOERROR)
		{
			/*
			** PassThruWriteMsgs failed! Get descriptive error string.
			*/
			PassThruGetLastError(&errstr[0]);
			LOG("WriteTest failed: %s",errstr);
			failed++;
		}

		ret=PassThruDisconnect(ChannelID);
		ParseReturnValue(ret);
		if (ret != STATUS_NOERROR)
			failed++;
	} else failed++;
	return failed;
}

int OtherTests()
{
	PASSTHRU_MSG Msg;
	PASSTHRU_MSG Msgs[16];
	unsigned long NumMsgs;
	char errstr[256];
	unsigned long int channelId;
	unsigned long ret;
	int failed=0;

	LOG("InterceptorTest 1:1");
	ret=PassThruConnect(SARDINE_DEVICE_ID,CAN,1<<8,SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (ret!=STATUS_NOERROR)
	{
		PassThruGetLastError(&errstr[0]);
		LOG("InterceptorTest failed: %s",errstr);
		failed++;
	} else
	{

		memset(&Msg, 0, sizeof(Msg));
		Msg.ProtocolID = CAN;
		// header: ECU
		Msg.Data[0] = 0x00;
		Msg.Data[1] = 0x0f;
		Msg.Data[2] = 0xff;
		Msg.Data[3] = 0xfe;
		// request heater status
		Msg.Data[4] = 0xcb;
		Msg.Data[5] = 0x40;
		Msg.Data[6] = 0xb9;
		Msg.Data[7] = 0xfb;
		Msg.DataSize=12;
		NumMsgs = 1;

		LOG("InterceptorTest 1:2");
		ret=PassThruWriteMsgs(channelId, &Msg, &NumMsgs, 1000);
		ParseReturnValue(ret);
		if (!ret)
		{
			LOG("InterceptorTest 1:3");
			memset(&Msgs, 0, sizeof(Msgs));

			Msgs[0].ProtocolID = CAN;
			Msgs[1].ProtocolID = CAN;
			Msgs[2].ProtocolID = CAN;
			Msgs[3].ProtocolID = CAN;
			Msgs[4].ProtocolID = CAN;
			Msgs[5].ProtocolID = CAN;
			Msgs[6].ProtocolID = CAN;
			Msgs[7].ProtocolID = CAN;
			Msgs[8].ProtocolID = CAN;
			Msgs[9].ProtocolID = CAN;
			Msgs[10].ProtocolID = CAN;
			Msgs[11].ProtocolID = CAN;
			Msgs[12].ProtocolID = CAN;

			/*
			** Indicate that PASSTHRU_MSG array contains 13 messages.
			*/
			NumMsgs = 13;

			/*
			** API/DLL should read first two messages in receive queue and immediately return. If there aren’t any
			** messages in the receive queue then API/DLL will return ERR_BUFFER_EMPTY.
			*/
			int Timeout = 1000;
			LOG("InterceptorTest 1:4");
			ret = PassThruReadMsgs(channelId, &Msgs[0], &NumMsgs, Timeout);
			ParseReturnValue(ret);
			if (ret != STATUS_NOERROR)
			{
				/*
				** PassThruReadMsgs failed! Get descriptive error string.
				*/
				PassThruGetLastError(&errstr[0]);
				LOG("ReadMsgs failed: %s",errstr);
				failed++;
			}
			ret=PassThruDisconnect(channelId);
			ParseReturnValue(ret);
			if (ret!=STATUS_NOERROR)
				failed++;
		} else failed++;
	}

	LOG("PeriodicMsgTest 1:1");
	ret=PassThruConnect(SARDINE_DEVICE_ID,CAN,1<<8,SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (ret!=STATUS_NOERROR)
	{
		PassThruGetLastError(&errstr[0]);
		LOG("OtherTest failed: %s",errstr);
	} else
	{

		memset(&Msg, 0, sizeof(Msg));
		Msg.ProtocolID = CAN;
		// header: ECU
		Msg.Data[0] = 0x00;
		Msg.Data[1] = 0x0f;
		Msg.Data[2] = 0xff;
		Msg.Data[3] = 0xfe;
		// request heater status
		Msg.Data[4] = 0xcd;
		Msg.Data[5] = 0x40;
		Msg.Data[6] = 0xa6;
		Msg.Data[7] = 0x5f;
		Msg.Data[8] = 0x32;
		Msg.Data[9] = 0x01;
		Msg.Data[10] = 0x00;
		Msg.Data[11] = 0x00;
		Msg.DataSize=12;

		LOG("PeriodicMsgTest 1:2");
		unsigned long msgId;
	ret=PassThruStartPeriodicMsg(channelId, &Msg, &msgId, 1000);
		ParseReturnValue(ret);
			if (!ret)
			{
			LOG("Starting to send periodic messages every 1000 ms (msg id 0x%x), press any key to stop",msgId);
			getchar();
			LOG("PeriodicMsgTest 1:3");
			ret=PassThruStopPeriodicMsg(channelId,msgId);
			ParseReturnValue(ret);
			}
	ret=PassThruDisconnect(channelId);

		ParseReturnValue(ret);
		if (ret != STATUS_NOERROR)
			failed++;
	}

	return failed;
}

int IOCTLTests()
{
	char errstr[256];
	unsigned long int channelId;
	unsigned long ret;
	SCONFIG CfgItem;
	SCONFIG_LIST Input;
	int failed = 0;

	LOG("IOCTLtest 1:1");
	ret=PassThruConnect(SARDINE_DEVICE_ID,CAN,1<<8,SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (ret!=STATUS_NOERROR)
	{
		PassThruGetLastError(&errstr[0]);
		LOG("IOCTLtest failed: %s",errstr);
		failed++;
	} else
	{
		CfgItem.Parameter = DATA_RATE;
		CfgItem.Value = 10400;
		Input.NumOfParams = 1;
		Input.ConfigPtr = &CfgItem;
		ret = PassThruIoctl(channelId, SET_CONFIG, (void *)&Input, (void *)NULL);
		if (ret != STATUS_NOERROR)
		{
			PassThruGetLastError(&errstr[0]);
			LOG("IOCTLtest failed: %s",errstr);
		}
		ret=PassThruDisconnect(channelId);
		ParseReturnValue(ret);
		if (ret != STATUS_NOERROR)
			failed++;
	}
	return failed;
}