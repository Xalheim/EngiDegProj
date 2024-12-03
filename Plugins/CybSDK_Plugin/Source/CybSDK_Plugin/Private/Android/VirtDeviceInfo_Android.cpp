#include "VirtDeviceInfo_Android.h"

FVirtDeviceInfo_Android::FVirtDeviceInfo_Android()
	: MajorVersion(0)
	, MinorVersion(0)
	, VendorId(0)
	, VendorName("")
	, ProductId(0)
	, ProductName("")
	, m_devicePath(nullptr)
{
}

FVirtDeviceInfo_Android::FVirtDeviceInfo_Android(int32 majorVersion, int32 minorVersion, int32 vendorId, FString vendorName, int32 productId, FString productName)
	: FVirtDeviceInfo_Android(majorVersion, minorVersion, vendorId, vendorName, productId, productName, nullptr)
{
}

FVirtDeviceInfo_Android::FVirtDeviceInfo_Android(int32 majorVersion, int32 minorVersion, int32 vendorId, FString vendorName, int32 productId, FString productName, char* devicePath)
	: MajorVersion(majorVersion)
	, MinorVersion(minorVersion)
	, VendorId(vendorId)
	, VendorName(vendorName)
	, ProductId(productId)
	, ProductName(productName)
	, m_devicePath(nullptr)
{
	const char* localDevicePath = devicePath;
	if (localDevicePath == nullptr)
		return;

	int32 SrcLen = TCString<char>::Strlen(localDevicePath) + 1;
	m_devicePath = new char[SrcLen];
	TCString<char>::Strcpy(m_devicePath, SrcLen, localDevicePath);
}

FVirtDeviceInfo_Android::~FVirtDeviceInfo_Android()
{
	delete[] m_devicePath;
	m_devicePath = nullptr;
}

const char* FVirtDeviceInfo_Android::GetDevicePath() const
{
	return m_devicePath;
}