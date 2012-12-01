
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

