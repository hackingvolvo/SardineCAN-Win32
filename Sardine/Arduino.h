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

#define ARDUINO_DEFAULT_COM_PORT 3
#define ARDUINO_DEFAULT_BAUD_RATE 115200
#define ARDUINO_DEFAULT_DTR_DISABLED 1

typedef BOOL (WINAPI *LPARDUINOLISTENER)( char * msg, int len, void * data );

namespace Arduino {

	int OpenDevice( int com_port, int baud_rate, int disable_DTR );
	bool IsConnected();
	int CloseDevice();
	int Listen( HANDLE CommEventHandle );	// non-blocking

	int RegisterListener( LPARDUINOLISTENER listener, void * data );
	void RemoveListener( LPARDUINOLISTENER listener );
	int Send(const char * msg);

	int HandleCommEvent();


}

