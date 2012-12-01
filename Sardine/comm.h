#include "stdafx.h"
#include <Windows.h>

namespace Comm {

	bool createCommThread();
	bool closeCommThread();

	bool WaitUntilInitialized( unsigned long timeout );
	
	void RequestInitialization();
	void SetInitialized();

	int WaitForEvents();	// blocks until event occurs (read)

}