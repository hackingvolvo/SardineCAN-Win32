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

#include "stdafx.h"
#include "Sardine.h"
#include "helper.h"
#include "comm.h"

bool setup()
{
	LOG(INIT,"Sardine: setup");
	return Comm::createCommThread();
}

void exitdll()
{
	LOG(INIT,"Sardine: Exitdll");
	Comm::closeCommThread();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	LOG(INIT,"Sardine v0.3 : (c) Olaf @ HackingVolvo Labs 2012");
	LOG(INIT,"DllMain: %d",ul_reason_for_call);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!setup())
			return FALSE;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		exitdll();
		break;
	}
	LOG(INIT,"Sardine: DllMain exit with return TRUE");
	return TRUE;
}

