#pragma once
#include "sardine_defs.h"

// protocol Ids
/*J1850VPW ……………... GM / DaimlerChrysler CLASS2 ………………………………0x01
J1850PWM ……………...Ford SCP(Standard Corporate Protocol) ………………………0x02
ISO9141 …………………ISO9141 and ISO9141-2 ………………………………………0x03
ISO14230 ………………. ISO14230-4(Keyword Protocol 2000) ……………………….. 0x04
CAN ……………………. Raw CAN(custom flow control implemented in software) ……0x05
ISO15765 ………………. ISO15765-2(CAN physical and data link layers with
Network layer flow control) ………………..0x06
SCI_A_ENGINE ……….. J2610(DaimlerChrysler Serial Communications Interface)
Configuration A for engine ……………………………………0x07
SCI_A_TRANS …………J2610(DaimlerChrysler Serial Communications Interface)
Configuration A for transmission ………………………….. 0x08
SCI_B_ENGINE…………J2610(DaimlerChrysler Serial Communications Interface)
Configuration B for engine ………………………………… 0x09
SCI_B_TRANS…………..J2610(DaimlerChrysler Serial Communications Interface)
Configuration B for transmission ………………………….. 0x0A
UNUSED ………………..Reserved ……………………………………………………… 0x0B -
0xFFFF
UNUSED ………………..PassThru tool vendor specific …………………………………0x10000 -
0xFFFFFFFF
*/




#define DllExport extern "C" long __stdcall
//extern "C"__declspec(dllexport) long __stdcall

#define J2534_API __stdcall

// J2534 Functions
extern "C" {
	long J2534_API PassThruOpen(void *pName, unsigned long *pDeviceID);
	long J2534_API PassThruClose(unsigned long DeviceID);
	long J2534_API PassThruConnect(unsigned long DeviceID, unsigned long ProtocolID, unsigned long Flags, unsigned long Baudrate, unsigned long *pChannelID);
	long J2534_API PassThruDisconnect(unsigned long ChannelID);
	long J2534_API PassThruReadMsgs(unsigned long ChannelID, PASSTHRU_MSG *pMsg, unsigned long *pNumMsgs, unsigned long Timeout);
	long J2534_API PassThruWriteMsgs(unsigned long ChannelID, PASSTHRU_MSG *pMsg, unsigned long *pNumMsgs, unsigned long Timeout);
	long J2534_API PassThruStartPeriodicMsg(unsigned long ChannelID, PASSTHRU_MSG * pMsg, unsigned long *pMsgID, unsigned long TimeInterval);
	long J2534_API PassThruStopPeriodicMsg(unsigned long ChannelID, unsigned long MsgID);
	long J2534_API PassThruStartMsgFilter(unsigned long ChannelID, unsigned long FilterType, PASSTHRU_MSG *pMaskMsg, PASSTHRU_MSG *pPatternMsg, PASSTHRU_MSG *pFlowControlMsg, unsigned long *pFilterID);
	long J2534_API PassThruStopMsgFilter(unsigned long ChannelID, unsigned long FilterID);
	long J2534_API PassThruSetProgrammingVoltage(unsigned long DeviceID, unsigned long PinNumber, unsigned long Voltage);
	long J2534_API PassThruReadVersion(unsigned long DeviceID, char *pFirmwareVersion, char *pDllVersion, char *pApiVersion);
	long J2534_API PassThruGetLastError(char *pErrorDescription);
	long J2534_API PassThruIoctl(unsigned long ChannelID, unsigned long IoctlID, void *pInput, void *pOutput);
}
/*
DllExport PassThruOpen(void *pName, unsigned long *pDeviceID);
DllExport PassThruClose(unsigned long pDeviceID);
DllExport PassThruConnect(unsigned long DeviceID, unsigned long ProtocolID, unsigned long Flags, unsigned long Baudrate, unsigned long *pChannelID)
DllExport PassThruDisconnect(unsigned long ChannelID);
DllExport PassThruReadMsgs(unsigned long ChannelID, void * pMsg, unsigned long * pNumMsgs, unsigned long Timeout);
DllExport PassThruWriteMsgs(unsigned long ChannelID, void * pMsg, unsigned long *pNumMsgs, unsigned long Timeout);
DllExport PassThruStartPeriodicMsg(unsigned long ChannelID, void * pMsg, unsigned long * pMsgID, unsigned long TimeInterval);
DllExport PassThruStopPeriodicMsg(unsigned long ChannelID, unsigned long pMsgID);
DllExport PassThruStartMsgFilter(unsigned long ChannelID, unsigned long FilterType, void * pMaskMsg, void * pPatternMsg, void * pFlowControlMsg, unsigned long * pFilterID);
DllExport PassThruStopMsgFilter(unsigned long ChannelID, unsigned long pFilterID);
DllExport PassThruSetProgrammingVoltage(unsigned long PinNumber, unsigned long Voltage);
DllExport PassThruReadVersion(char * pFirmwareVersion, char *pDllVersion, char *pApiVersion);
DllExport PassThruGetLastError(char *pErrorDescription);
DllExport PassThruIoctl(unsigned long ChannelID, unsigned long IoctlID, void * pInput, void * pOutput);
*/

/*
//Other Functions
WINAPI LoadJ2534Dll(char *);
WINAPI UnloadJ2534Dll();
*/

// NCS Returns of any functions not found
#define ERR_NO_PTOPEN					0x0001
#define ERR_NO_PTCLOSE					0x0002
#define ERR_NO_PTCONNECT				0x0004
#define ERR_NO_PTDISCONNECT				0x0008
#define ERR_NO_PTREADMSGS				0x0010
#define ERR_NO_PTWRITEMSGS				0x0020
#define ERR_NO_PTSTARTPERIODICMSG		0x0040
#define ERR_NO_PTSTOPPERIODICMSG		0x0080
#define ERR_NO_PTSTARTMSGFILTER			0x0100
#define ERR_NO_PTSTOPMSGFILTER			0x0200
#define ERR_NO_PTSETPROGRAMMINGVOLTAGE	0x0400
#define ERR_NO_PTREADVERSION			0x0800
#define ERR_NO_PTGETLASTERROR			0x1000
#define ERR_NO_PTIOCTL					0x2000
#define ERR_NO_FUNCTIONS				0x3fff
#define ERR_NO_DLL						-1
#define ERR_WRONG_DLL_VER				-2
#define ERR_FUNC_MISSING				-3

