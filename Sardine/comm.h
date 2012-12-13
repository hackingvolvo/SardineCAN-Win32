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
#include <Windows.h>

// Handles the communication thread and messaging between it and the main DLL thread
namespace Comm {

	bool createCommThread();
	bool closeCommThread();

	bool WaitUntilInitialized( unsigned long timeout );
	
	void RequestInitialization();
	void SetInitialized();

	int WaitForEvents();	// blocks until event occurs (read)

}