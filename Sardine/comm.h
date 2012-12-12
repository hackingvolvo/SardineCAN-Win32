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