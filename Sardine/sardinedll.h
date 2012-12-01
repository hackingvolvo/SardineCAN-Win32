#pragma once
#include "j2534_v0404.h"

/*
** Define all the PassThru function prototypes for the J2534 API.
*/

/*
typedef long (CALLBACK* PTOPEN)(void *pName, unsigned long *pDeviceID);
typedef long (CALLBACK* PTCLOSE)(unsigned long pDeviceID);
typedef long (CALLBACK* PTCONNECT)(unsigned long ProtocolID, unsigned long Flags, unsigned long *pChannelID);
typedef long (CALLBACK* PTDISCONNECT)(unsigned long ChannelID);
typedef long (CALLBACK* PTREADMSGS)(unsigned long ChannelID, void * pMsg, unsigned long * pNumMsgs, unsigned long Timeout);
typedef long (CALLBACK* PTWRITEMSGS)(unsigned long ChannelID, void *  pMsg, unsigned long * pNumMsgs, unsigned long Timeout);
typedef long (CALLBACK* PTSTARTPERIODICMSG)(unsigned long ChannelID, void * pMsg, unsigned long * pMsgID, unsigned long TimeInterval);
typedef long (CALLBACK* PTSTOPPERIODICMSG)(unsigned long ChannelID, unsigned long MsgID);
typedef long (CALLBACK* PTSTARTMSGFILTER)(unsigned long ChannelID, unsigned long FilterType, void * pMaskMsg, void * pPatternMsg, void * pFlowControlMsg, unsigned long * pFilterID);
typedef long (CALLBACK* PTSTOPMSGFILTER)(unsigned long ChannelID, unsigned long FilterID);
typedef long (CALLBACK* PTSETPROGRAMMINGVOLTAGE)(unsigned long PinNumber, unsigned long Voltage);
typedef long (CALLBACK* PTREADVERSION)(char *pFirmwareVersion, char *pDllVersion, char *pApiVersion);
typedef long (CALLBACK* PTGETLASTERROR)(char *pErrorDescription);
typedef long (CALLBACK* PTIOCTL)(unsigned long ChannelID, unsigned long IoctlID, void *pInput, void * pOutput);
*/
/*
#define J2534_API __stdcall

typedef long (J2534_API* PTOPEN)(void *pName, unsigned long *pDeviceID);
typedef long (J2534_API* PTCLOSE)(unsigned long DeviceID);
typedef long (J2534_API* PTCONNECT)(unsigned long DeviceID, unsigned long ProtocolID, unsigned long Flags, unsigned long BaudRate, unsigned long *pChannelID);
typedef long (J2534_API* PTDISCONNECT)(unsigned long ChannelID);
typedef long (J2534_API* PTREADMSGS)(unsigned long ChannelID, PASSTHRU_MSG *pMsg, unsigned long *pNumMsgs, unsigned long Timeout);
typedef long (J2534_API* PTWRITEMSGS)(unsigned long ChannelID, PASSTHRU_MSG *pMsg, unsigned long *pNumMsgs, unsigned long Timeout);
typedef long (J2534_API* PTSTARTPERIODICMSG)(unsigned long ChannelID, PASSTHRU_MSG *pMsg, unsigned long *pMsgID, unsigned long TimeInterval);
typedef long (J2534_API* PTSTOPPERIODICMSG)(unsigned long ChannelID, unsigned long MsgID);
typedef long (J2534_API* PTSTARTMSGFILTER)(unsigned long ChannelID, unsigned long FilterType, PASSTHRU_MSG *pMaskMsg, PASSTHRU_MSG *pPatternMsg, PASSTHRU_MSG *pFlowControlMsg, unsigned long *pFilterID);
typedef long (J2534_API* PTSTOPMSGFILTER)(unsigned long ChannelID, unsigned long FilterID);
typedef long (J2534_API* PTSETPROGRAMMINGVOLTAGE)(unsigned long DeviceID, unsigned long PinNumber, unsigned long Voltage);
typedef long (J2534_API* PTREADVERSION)(unsigned long DeviceID, char *pFirmwareVersion, char *pDllVersion, char *pApiVersion);
typedef long (J2534_API* PTGETLASTERROR)(char *pErrorDescription);
typedef long (J2534_API* PTIOCTL)(unsigned long ChannelID, unsigned long IoctlID, void *pInput, void *pOutput);
*/

/*
** Define permanent storage for all the PassThru function addresses.
*/
extern PTOPEN	PassThruOpen;
extern PTCLOSE	PassThruClose;
extern PTCONNECT PassThruConnect;
extern PTDISCONNECT PassThruDisconnect;
extern PTREADMSGS PassThruReadMsgs;
extern PTWRITEMSGS PassThruWriteMsgs;
extern PTSTARTPERIODICMSG PassThruStartPeriodicMsg;
extern PTSTOPPERIODICMSG PassThruStopPeriodicMsg;
extern PTSTARTMSGFILTER PassThruStartMsgFilter;
extern PTSTOPMSGFILTER PassThruStopMsgFilter;
extern PTSETPROGRAMMINGVOLTAGE PassThruSetProgrammingVoltage;
extern PTREADVERSION PassThruReadVersion;
extern PTGETLASTERROR PassThruGetLastError;
extern PTIOCTL PassThruIoctl;

