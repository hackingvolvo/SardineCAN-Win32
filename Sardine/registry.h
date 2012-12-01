#pragma once
#include "stdafx.h"

namespace SardineRegistry {

bool GetSettingsFromRegistry( int * ComPort, int * BaudRate, int * disableDTR );
bool GetValueFromRegistry( HKEY previousKey, TCHAR * valueName, unsigned long * value );

}