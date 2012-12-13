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
	unsigned long ret;
	unsigned long channelId; 
	unsigned long NumMsgs;
	PASSTHRU_MSG Msg[2];
	unsigned long Timeout;
	char errstr[256];
	int failed=0;

	LOG("ReadTest 1:1");
	ret = PassThruConnect(SARDINE_DEVICE_ID,ISO15765, 0x00000000, SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (!ret)
	{
		memset(&Msg, 0, sizeof(Msg));

		Msg[0].ProtocolID = ISO15765;
		Msg[1].ProtocolID = ISO15765;

		NumMsgs = 2;

		Timeout = 0;
		LOG("ReadTest 1:2");
		ret = PassThruReadMsgs(channelId, &Msg[0], &NumMsgs, Timeout);
		ParseReturnValue(ret);
		if (ret != STATUS_NOERROR)
		{
			PassThruGetLastError(&errstr[0]);
			LOG("ReadMsgs failed: %s",errstr);
			failed++;
		} else
		{
			for (unsigned int i=0;i<NumMsgs;i++)
			{
				printf("Reply msg %d: ",i);
				for (unsigned int j=0;j<Msg[i].DataSize;j++)
					printf("%02x ",Msg[i].Data[j]);
				printf("\n");
			}
		}
		ret=PassThruDisconnect(channelId);
		ParseReturnValue(ret);
		if (ret != STATUS_NOERROR)
			failed++;
	} else failed++;


	LOG("ReadTest 2:1");
	ret = PassThruConnect(SARDINE_DEVICE_ID,CAN, 0x00000000, SARDINE_DEFAULT_CAN_BAUD_RATE,&channelId);
	ParseReturnValue(ret);
	if (!ret)
	{
		memset(&Msg, 0, sizeof(Msg));

		Msg[0].ProtocolID = CAN;
		Msg[1].ProtocolID = CAN;
		NumMsgs = 2;

		Timeout = 0;
		LOG("ReadTest 2:2");
		ret = PassThruReadMsgs(channelId, &Msg[0], &NumMsgs, Timeout);
		if (ret != STATUS_NOERROR)
		{
			PassThruGetLastError(&errstr[0]);
			LOG("ReadMsgs failed: %s",errstr);
		}

		// second read, but wait two seconds for msgs
		Timeout = 2000;
		LOG("ReadTest 3:2");
		ret = PassThruReadMsgs(channelId, &Msg[0], &NumMsgs, Timeout);
		if (ret != STATUS_NOERROR)
		{
			PassThruGetLastError(&errstr[0]);
			LOG("ReadMsgs failed: %s",errstr);
			failed++;
		} else
		{
			for (unsigned int i=0;i<NumMsgs;i++)
			{
				printf("Reply msg %d: ",i);
				for (unsigned int j=0;j<Msg[i].DataSize;j++)
					printf("%02x ",Msg[i].Data[j]);
				printf("\n");
			}
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
	unsigned long ChannelID; 
	unsigned long FilterID;

	unsigned long ret;
	unsigned long NumMsgs;
	PASSTHRU_MSG Msg[1];
	unsigned long Timeout;
	char errstr[256];

	LOG("WriteTest 1:1");
	ret = PassThruConnect(SARDINE_DEVICE_ID,ISO15765, 0x00000000, SARDINE_DEFAULT_CAN_BAUD_RATE, &ChannelID);
	ParseReturnValue(ret);
	if (!ret)
	{

		CfgItem[0].Parameter = ISO15765_BS;
		CfgItem[0].Value = 0x20; 
		CfgItem[1].Parameter = ISO15765_STMIN;
		CfgItem[1].Value = 0x01; 
		Input.NumOfParams = 2; 
		Input.ConfigPtr = &CfgItem[0];

		status = PassThruIoctl(ChannelID, SET_CONFIG, (void *)&Input, NULL);

		memset(&MaskMsg, 0, sizeof(MaskMsg));
		MaskMsg.ProtocolID = ISO15765;
		MaskMsg.Data[0] = 0xff;
		MaskMsg.Data[1] = 0xff;
		MaskMsg.Data[2] = 0xff;
		MaskMsg.Data[3] = 0xff;
		MaskMsg.DataSize = 4;

		memset(&PatternMsg, 0, sizeof(PatternMsg));
		PatternMsg.ProtocolID = ISO15765;
		PatternMsg.Data[0] = 0x00;
		PatternMsg.Data[1] = 0x80;
		PatternMsg.Data[2] = 0x00;
		PatternMsg.Data[3] = 0x03;
		PatternMsg.DataSize = 4;

		memset(&FilterMsg, 0, sizeof(FilterMsg));
		FilterMsg.ProtocolID = ISO15765;
		FilterMsg.Data[0] = 0x00;
		FilterMsg.Data[1] = 0x0f;
		FilterMsg.Data[2] = 0xff;
		FilterMsg.Data[3] = 0xfe;
		FilterMsg.DataSize = 4; 
		status = PassThruStartMsgFilter(ChannelID, FLOW_CONTROL_FILTER, &MaskMsg, &PatternMsg,
			&FilterMsg, &FilterID);
		if (status != STATUS_NOERROR)
		{
			PassThruGetLastError(&errstr[0]);
			failed++;
		}

		memset(&Msg, 0, sizeof(Msg));

		Msg[0].ProtocolID = ISO15765;

		Msg[0].Data[0] = 0x00;
		Msg[0].Data[1] = 0x0f;
		Msg[0].Data[2] = 0xff;
		Msg[0].Data[3] = 0xfe;
		Msg[0].DataSize = 4;

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

		NumMsgs = 1;

		Timeout = 0;
		LOG("WriteTest 1:2");
		ret = PassThruWriteMsgs(ChannelID, &Msg[0], &NumMsgs, Timeout);
		if (ret != STATUS_NOERROR)
		{
			PassThruGetLastError(&errstr[0]);
			LOG("WriteTest failed: %s",errstr);
			failed++;
		}

		// Wait one second for reply
		Timeout = 1000;
		NumMsgs = 1;
		LOG("WriteTest 1:3");
		ret = PassThruReadMsgs(ChannelID, &Msg[0], &NumMsgs, Timeout);
		if (ret != STATUS_NOERROR)
		{
			PassThruGetLastError(&errstr[0]);
			LOG("ReadMsgs failed: %s",errstr);
			failed++;
		} else
		{
			for (unsigned int i=0;i<NumMsgs;i++)
			{
				printf("Reply msg %d: ",i);
				for (unsigned int j=0;j<Msg[i].DataSize;j++)
					printf("%02x ",Msg[i].Data[j]);
				printf("\n");
			}
		}
		ret=PassThruDisconnect(ChannelID);
		ParseReturnValue(ret);
		if (ret != STATUS_NOERROR)
			failed++;
	} else failed++;


	LOG("WriteTest 2:1");
	ret = PassThruConnect(SARDINE_DEVICE_ID,CAN, 0x00000000,SARDINE_DEFAULT_CAN_BAUD_RATE, &ChannelID);
	ParseReturnValue(ret);
	if (!ret)
	{
		memset(&Msg, 0, sizeof(Msg));

		Msg[0].ProtocolID = CAN;

		// header: Tester ID
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

		NumMsgs = 1;

		Timeout = 0;
		LOG("WriteTest 2:2");
		ret = PassThruWriteMsgs(ChannelID, &Msg[0], &NumMsgs, Timeout);
		if (ret != STATUS_NOERROR)
		{
			PassThruGetLastError(&errstr[0]);
			LOG("WriteTest failed: %s",errstr);
			failed++;
		}
		// Wait one second for reply
		Timeout = 1000;
		NumMsgs = 1;
		LOG("WriteTest 3:3");
		ret = PassThruReadMsgs(ChannelID, &Msg[0], &NumMsgs, Timeout);
		if (ret != STATUS_NOERROR)
		{
			PassThruGetLastError(&errstr[0]);
			LOG("ReadMsgs failed: %s",errstr);
			failed++;
		} else
		{
			for (unsigned int i=0;i<NumMsgs;i++)
			{
				printf("Reply msg %d: ",i);
				for (unsigned int j=0;j<Msg[i].DataSize;j++)
					printf("%02x ",Msg[i].Data[j]);
				printf("\n");
			}
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

	LOG("InterceptorTest 1:1 - NOTE! For this to work, \"UseInterceptor\" must be set to \"1\" in Windows registry!");
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

		// header: Tester
		Msg.Data[0] = 0x00;
		Msg.Data[1] = 0x0f;
		Msg.Data[2] = 0xff;
		Msg.Data[3] = 0xfe;

		// request VIN
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

			NumMsgs = 13;

			int Timeout = 1000;
			LOG("InterceptorTest 1:4");
			ret = PassThruReadMsgs(channelId, &Msgs[0], &NumMsgs, Timeout);
			ParseReturnValue(ret);
			if (ret != STATUS_NOERROR)
			{
				PassThruGetLastError(&errstr[0]);
				LOG("ReadMsgs failed: %s",errstr);
				failed++;
			} else
			{
				for (unsigned int i=0;i<NumMsgs;i++)
				{
					printf("Reply msg %d: ",i);
					for (unsigned int j=0;j<Msgs[i].DataSize;j++)
						printf("%02x ",Msgs[i].Data[j]);
					printf("\n");
				}
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
		// header: Tester ID
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