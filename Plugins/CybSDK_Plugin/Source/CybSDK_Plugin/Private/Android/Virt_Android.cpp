#include "Virt_Android.h"

#include "CybSDK_Plugin.h"

int Virt_Android::GetSDKVersion()
{
	return 1;
}

UVirtDevice_Android* Virt_Android::FindDevice()
{
	UVirtDevice_Android* NewVirtDevice_Android = NewObject<UVirtDevice_Android>();
	NewVirtDevice_Android->Init();
	return NewVirtDevice_Android;
}
