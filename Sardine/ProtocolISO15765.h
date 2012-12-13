/*
**
** Copyright (C) 2012 Olaf @ Hacking Volvo blog (hackingvolvo.blogspot.com)
** Author: Olaf <hackingvolvo@gmail.com>
**
** This library is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation, either version 3 of the License, or (at
** your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, <http://www.gnu.org/licenses/>.
**
*/
#pragma once
#include "protocolcan.h"

#define MAX_FLOW_FILTERS 10
#define MAX_FC_SESSIONS 16

typedef struct {
	unsigned char pattern[12];
	unsigned char mask[12];
	unsigned char flowControlMsg[12];
	bool ext_addr;
	unsigned long id;
	unsigned long passFilterId; // associated pass filter (created by lower level StartMsgFilter)
	int len;
} flowFilter;

typedef struct {
	PASSTHRU_MSG * msg;		// message that is being assembled from separate frames of one multi-frame message
	unsigned int expected_length;	// length of whole message (sent in the first frame)
	unsigned int previousFrameSeqNumber;	// sequence number of the previous frame we have received
	unsigned int headerLen;		// 4 or 5 bytes
	unsigned int payloadLen;	// current size of payload assembled from frames up to now
	bool extended_addressing;
} flowControlSession;

class CProtocolISO15765 :
	public CProtocolCAN
{
public:
	CProtocolISO15765(int ProtocolID);
	~CProtocolISO15765(void);

	virtual int GetIOCTLParam( SCONFIG * pConfig );
	virtual int SetIOCTLParam( SCONFIG * pConfig );

	virtual int WriteMsg( PASSTHRU_MSG * pMsg, unsigned long Timeout );

	virtual int DeleteFilters();
	int StartMsgFilter( unsigned long FilterType, PASSTHRU_MSG * pMaskMsg, PASSTHRU_MSG * pPatternMsg, PASSTHRU_MSG * pFlowControlMsg, unsigned long * pFilterId );
	int StopMsgFilter(  unsigned long FilterID );
//	int IOCTL(unsigned long IoctlID, void *pInput, void *pOutput);

	bool HandleMsg( PASSTHRU_MSG * pMsg, char * flags);

private:

	int GetFilterById( unsigned long FilterId );
	int CreateFilter( PASSTHRU_MSG * pMaskMsg, PASSTHRU_MSG * pPatternMsg, PASSTHRU_MSG * pFlowControlMsg, unsigned long passFilterId );

	// Filter matching functions. Checks if the first 4 bytes (5 bytes if using extended addressing) of pMsg data field (masked by filter) matches the pattern/flow field 
	// If compareToFlowMsg==true, then we with compare the masked bytes with flowControlMsg bytes, otherwise with pattern bytes.
	bool DoesMatchFilter( PASSTHRU_MSG * pMsg, bool compareToFlowMsg );
	int GetMatchingFilter( PASSTHRU_MSG * pMsg,  bool compareToFlowMsg );


	// Session and flow control handlers. Session is created automatically when FIRST_FRAME of multi-frame message is received. Then a flow control message is sent to the sender 
	// and incoming frames buffered until last one is received. Then we put the assembled multi-frame to receive buffer.
	void DeleteSession( unsigned int sessionIdx );
	int GetSessionByMsg( PASSTHRU_MSG * pMsg );
	void StartSession( PASSTHRU_MSG * pFirstMsg );
	bool GenerateFlowControlMsg( flowFilter * filter, unsigned int flowStatus );
	bool HandleConsecutiveFrame( PASSTHRU_MSG * pMsg, unsigned char PCI );

	// stub for handling the incoming flow control message from other host after the first frame of our multi-frame message is sent. 
	// FIXME: not yet implemented flow control for sending.
	bool HandleIncomingFlowControlMessage( PASSTHRU_MSG * pMsg );

	flowFilter filters[MAX_FLOW_FILTERS];
	unsigned int filtersnum;
	flowControlSession sessions[MAX_FC_SESSIONS];
	unsigned int sessionsnum;

	int can_mixed_format;
	unsigned int _running_filter_id;
	unsigned int separation_time;
	unsigned int block_size;

};

