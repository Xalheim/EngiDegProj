#pragma once
#include "CoreMinimal.h"
#include "StandaloneVirtDeviceInterface.h"
#include "Tickable.h"
#include "Android/AndroidJava.h"
#include "Android/AndroidJNI.h"
#include "VirtDeviceInfo_Android.h"
#include "GameFramework/Actor.h"

#include "VirtDevice_Android.generated.h"

UCLASS()
class CYBSDK_PLUGIN_API UVirtDevice_Android : public UObject, public IStandaloneVirtDeviceInterface, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//****************************************************************************************
	//* Ctor/Dtor
	//****************************************************************************************
	
	virtual ~UVirtDevice_Android();

	//****************************************************************************************
	//* Java/Ble
	//****************************************************************************************

	/**
	 * @brief Initializes the Java VirtDevice object and starts the connection process if the
	 * Android permissions were already given.
	 */
	void Init();

	virtual TArray<FString> GetPairedDevices() override;
	virtual FString GetSavedVirtSamName() override;
	virtual void SaveToFileVirtSam(FString Name) override;
	virtual void FindAndConnectSavedVirtSam() override;
	virtual EBleConnectionStates GetCurrentState() override;
	virtual void BindToBleStateChangedDelegate(UObject* TargetObject, FName FuncName) override;
	virtual void SetOrientationOffset(float offset) override;

	void SetStateTo(EBleConnectionStates NewState);

	// Takes care of updating the haptic values without clogging the BLE operations queue.
	virtual void Tick(float DeltaTime) override;

protected:
	FString m_savedVirtSamName = "VirtSAM1";
	EBleConnectionStates m_state = Unknown;
	TArray<FString> m_pairedDevices;
	const FString m_fineLocationPermissionString = "android.permission.BLUETOOTH_CONNECT";
	
private:
	// Bluetooth is initialized either if permissions have already been given, or after
	// they have been given in the current session
	void InitBluetooth();	
	UFUNCTION()
	void OnAndroidPermission(const TArray<FString>& Permissions, const TArray<bool>& GrantResults);

	// Required for TickableGameObject implementation
	virtual TStatId GetStatId() const override;

	int m_gain = 0;
	int m_frequency = 0;
	int m_volume = 0;
	int m_lastGain = -1;
	int m_lastFrequency = -1;
	int m_lastVolume = -1;
	const float m_hapticPeriod = 0.085f;
	float m_lastHapticTime = 1.0f;
	
	float m_orientationOffset = 0.0f;

	// Java/Android interface
	TSharedPtr<FJavaClassObject> AndroidVirtDeviceObject;

	FJavaClassMethod InitMethod;
	FJavaClassMethod ExitMethod;
	FJavaClassMethod FindPairedVirtSamAndConnectMethod;
	FJavaClassMethod GetPairedDevicesMethod;
	FJavaClassMethod OnApplicationQuitMethod;

	FJavaClassMethod ConnectMethod;
	FJavaClassMethod OpenMethod;
	FJavaClassMethod IsOpenMethod;
	FJavaClassMethod CloseMethod;
	
	FJavaClassMethod GetMajorVersionMethod;
	FJavaClassMethod GetMinorVersionMethod;
	FJavaClassMethod GetVendorIdMethod;
	FJavaClassMethod GetVendorNameMethod;
	FJavaClassMethod GetProductIdMethod;
	FJavaClassMethod GetProductNameMethod;

	FJavaClassMethod GetPlayerHeightMethod;
	FJavaClassMethod ResetPlayerHeightMethod;
	FJavaClassMethod GetPlayerOrientationMethod;
	FJavaClassMethod ResetPlayerOrientationMethod;
	FJavaClassMethod GetMovementSpeedMethod;
	FJavaClassMethod GetMovementDirectionMethod;
	
	FJavaClassMethod HasHapticMethod;
	FJavaClassMethod HapticPlayMethod;
	FJavaClassMethod HapticStopMethod;
	FJavaClassMethod HapticSetValues;
	
public:
	//****************************************************************************************
	//* Connection
	//****************************************************************************************

	/**
	 * @brief Opens the connection to the Virtualizer device.
	 * @remark No other application can open this device at the same time.
	 * @return true if the connection was successfully opened, otherwise false.
	 */
	bool Open();
	/**
	 * @brief Checks if the connection was opened before.
	 */
	bool IsOpen() const;
	/**
	 * @brief Closes the connection to the Virtualizer device.
	 */
	bool Close();


	//****************************************************************************************
	//* Device Infos
	//****************************************************************************************

	/**
	 * @brief Returns the USB infos of this device.
	 */
	FVirtDeviceInfo_Android& GetDeviceInfo();

	//****************************************************************************************
	//* Virtualizer Game Data
	//****************************************************************************************

	/**
	 * @brief Returns the current player height relative to the default height.
	 * @remark The default height is set by the ResetPlayerHeight method.
	 * @remark height < -threshold: crouching
	 * @remark height >  threshold: jumping
	 * @return 1.00f = 1cm.
	 */
	float GetPlayerHeight() const;

	/**
	 * @brief Assigns the current height to the default height.
	 * @remark This method should be called while the player is asked to stand upright.
	 */
	void ResetPlayerHeight();

	/**
	 * @brief Returns the orientation of the player as an absolute value.
	 * @remark The origin is set by the ResetPlayerOrientation method and increases clockwise.
	 * @return logical: 0.00f to 1.00f (= physical: 0° to 360°).
	 */
	float GetPlayerOrientation() const;

	/**
	 * @brief Assigns the current orientation to the default vector.
	 * @remark This method should be called while the player is asked to look forward.
	 * @remark This orientation should be used to calibrate the HMD.
	 * @deprecated Unused as new Virtualizers orient themselves automatically.
	 */
	void ResetPlayerOrientation();

	/**
	 * @brief Returns the current movement speed in meters per second.
	 * @return 1.00f = 1m/s
	 */
	float GetMovementSpeed() const;

	/**
	 * @brief Returns the movement direction relative to the current player orientation.
	 * @remark The origin is the GetPlayerOrientation method and increases clockwise.
	 * @return logical: -1.00f to 1.00f (= physical: -180° to 180°).
	 */
	float GetMovementDirection() const;

	//****************************************************************************************
	//* Haptic
	//****************************************************************************************

	/**
	 * @brief Checks if the Virtualizer device supports haptic feedback.
	 */
	bool HasHaptic() const;

	/**
	 * @brief Play a signal on the haptic unit.
	 */
	void HapticPlay();

	/**
	 * @brief Stop the haptic unit.
	 */
	void HapticStop();

	/**
	 * @brief Set the gain (dB) level of the haptic unit.
	 * @param gain The value can be 0, 1, 2 or 3.
	 */
	void HapticSetGain(int gain);

	/**
	 * @brief Set the frequency (Hz) of a sine wave on the haptic unit.
	 * @param frequency The value is valid between 10Hz and 80Hz.
	 */
	void HapticSetFrequency(int frequency);

	/**
	 * @brief Sets the haptic feedback (change of amplitude) in the baseplate.
	 * @param volume The value is valid between 0 (no feedback) and 100 (full feedback).
	 */
	void HapticSetVolume(int volume);

private:	
	FVirtDeviceInfo_Android VirtDeviceInfo;
};
