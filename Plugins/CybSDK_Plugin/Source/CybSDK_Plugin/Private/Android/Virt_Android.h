#pragma once
#include "VirtDevice_Android.h"

class CYBSDK_PLUGIN_API Virt_Android
{
public:
	// Returns 0 for now
	static int GetSDKVersion();
	// Returns the Android VirtDevice
	static UVirtDevice_Android* FindDevice();
};
