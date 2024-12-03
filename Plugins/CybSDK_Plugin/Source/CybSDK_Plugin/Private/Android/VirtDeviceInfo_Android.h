#pragma once
#include "Runtime/Core/Public/CoreMinimal.h"
#include "VirtDeviceInfo_Android.generated.h"

USTRUCT()
struct FVirtDeviceInfo_Android
{
	GENERATED_BODY()
public:
	/**
		 * \brief Default Constructor
		 */
	FVirtDeviceInfo_Android();
	/**
	 * \brief Constructor for a BLE-Connected Virtualizer device
	 */
	FVirtDeviceInfo_Android(int32 majorVersion, int32 minorVersion, int32 vendorId, FString vendorName, int32 productId, FString productName);
	/**
	* \brief Constructor for a Virtualizer device
	*/
	FVirtDeviceInfo_Android(int32 majorVersion, int32 minorVersion, int32 vendorId, FString vendorName, int32 productId, FString productName, char* devicePath);
	/**
	 * \brief Destructor
	 */
	~FVirtDeviceInfo_Android();
	
	/**
	 * @brief The major firmware version.
	 */
	int32 MajorVersion;
	
	/**
	 * @brief The minor firmware version.
	 */
	int32 MinorVersion;

	/**
	 * @brief The USB vendor id.
	 */
	int32 VendorId;
	/**
	 * @brief The USB vendor name.
	 */
	FString VendorName;
	
	/**
	 * @brief The USB product id.
	 */
	int32 ProductId;
	/**
	 * @brief The USB product name.
	 */
	FString ProductName;

	/**
		 * @brief Returns the USB device path used for the HID connection.
		 */
	const char* GetDevicePath() const;

protected:
	/**
	 * @brief The UBS device path used for the HID connection.
	 */
	char* m_devicePath;
};
