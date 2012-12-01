/*
**
** Copyright (C) 2009 Drew Technologies Inc.
** Author: Joey Oravec <joravec@drewtech.com>
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

#include "j2534_v0404.h"
#include <tchar.h>

//#include "shim_loader.h" // for tstring
#include <string>

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

void shim_setInternalError(LPCTSTR szError, ...);
LPCTSTR shim_getInternalError();
void shim_clearInternalError();
bool shim_hadInternalError();

tstring dbug_return(unsigned long RetVal);
tstring dbug_filter(unsigned long FilterType);
tstring dbug_ioctl(unsigned long IoctlID);
tstring dbug_param(unsigned long ParamID);
tstring dbug_prot(unsigned long ProtocolID);

void dbug_printcflag(unsigned long ConnectFlags);
void dbug_printrxstatus(unsigned long RxStatus);
void dbug_printtxflags(unsigned long TxFlags);
void dbug_printretval(unsigned long RetVal);
void dbug_printsbyte(SBYTE_ARRAY *inAry, LPCTSTR s);
void dbug_printsconfig(SCONFIG_LIST *pList);
void dbug_printmsg(PASSTHRU_MSG mm[], LPCTSTR s, unsigned long * numMsgs, bool isWrite);
void dbug_printmsg(PASSTHRU_MSG mm[], LPCTSTR s, unsigned long numMsgs, bool isWrite);

LPCTSTR dbug_ioctl2str(unsigned long IoctlID);
LPCTSTR dbug_param2str(unsigned long ParamID);

LPCTSTR dbug_filter2str(unsigned long FilterType);


