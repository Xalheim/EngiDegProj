#include "VirtDevice_Android.h"
#include "AndroidPermissionCallbackProxy.h"
#include "AndroidPermissionFunctionLibrary.h"
#include "Async/Async.h"
#include "CybSDK_Plugin.h"
#include "Utils/JavaTypesConvertor.h"
#include "Utils/SavedVirtDataManager.h"
#include "Engine/World.h"
#include "HeadMountedDisplayFunctionLibrary.h"

void UVirtDevice_Android::OnAndroidPermission(const TArray<FString>& Permissions, const TArray<bool>& GrantResults)
{
	for (int i = 0 ; i < Permissions.Num() ; i++)
	{
		if (Permissions[i] == m_fineLocationPermissionString)
		{
			if (GrantResults[i])
			{
				FFunctionGraphTask::CreateAndDispatchWhenReady([this]()
				{
					this->InitBluetooth();
				}, TStatId(), nullptr, ENamedThreads::GameThread);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[CYBERITH] Android permission for ACCESS_FINE_LOCATION was denied, Bluetooth cannot be initialized"));
			}
		}
	}
}

UVirtDevice_Android::~UVirtDevice_Android()
{
	AndroidVirtDeviceObject->CallMethod<void>(ExitMethod);
}

void UVirtDevice_Android::Init()
{	
	AndroidVirtDeviceObject = MakeShared<FJavaClassObject>("com/cyberith/unrealengineble/AndroidVirtDevice",
		"(Landroid/app/Activity;J)V",
		FJavaWrapper::GameActivityThis,
		(jlong)this);

	InitMethod = AndroidVirtDeviceObject->GetClassMethod("init", "()V");
	ExitMethod = AndroidVirtDeviceObject->GetClassMethod("exit", "()V");
	FindPairedVirtSamAndConnectMethod = AndroidVirtDeviceObject->GetClassMethod("findPairedVirtSamAndConnect", "(Ljava/lang/String;)[Ljava/lang/String;");

	OpenMethod = AndroidVirtDeviceObject->GetClassMethod("open", "()Z");
	IsOpenMethod = AndroidVirtDeviceObject->GetClassMethod("isOpen", "()Z");
	CloseMethod = AndroidVirtDeviceObject->GetClassMethod("close", "()Z");
	
	GetMajorVersionMethod = AndroidVirtDeviceObject->GetClassMethod("getMajorVersion", "()I");
	GetMinorVersionMethod = AndroidVirtDeviceObject->GetClassMethod("getMinorVersion", "()I");
	GetVendorIdMethod = AndroidVirtDeviceObject->GetClassMethod("getVendorId", "()I");
	GetVendorNameMethod = AndroidVirtDeviceObject->GetClassMethod("getVendorName", "()Ljava/lang/String;");
	GetProductIdMethod = AndroidVirtDeviceObject->GetClassMethod("getProductId", "()I");
	GetProductNameMethod = AndroidVirtDeviceObject->GetClassMethod("getProductName", "()Ljava/lang/String;");

	GetPlayerHeightMethod = AndroidVirtDeviceObject->GetClassMethod("getPlayerHeight", "()I");
	ResetPlayerHeightMethod = AndroidVirtDeviceObject->GetClassMethod("resetPlayerHeight", "()V");
	GetPlayerOrientationMethod = AndroidVirtDeviceObject->GetClassMethod("getPlayerOrientation", "()I");
	ResetPlayerOrientationMethod = AndroidVirtDeviceObject->GetClassMethod("resetPlayerOrientation", "()V");
	GetMovementSpeedMethod = AndroidVirtDeviceObject->GetClassMethod("getMovementSpeed", "()I");
	GetMovementDirectionMethod = AndroidVirtDeviceObject->GetClassMethod("getMovementDirection", "()I");
	
	HasHapticMethod = AndroidVirtDeviceObject->GetClassMethod("hasHaptic", "()Z");
	HapticPlayMethod = AndroidVirtDeviceObject->GetClassMethod("hapticPlay", "()V");
	HapticStopMethod = AndroidVirtDeviceObject->GetClassMethod("hapticStop", "()V");
	HapticSetValues = AndroidVirtDeviceObject->GetClassMethod("hapticSetValues", "(III)V");

//	InitBluetooth();
	
	UAndroidPermissionCallbackProxy::GetInstance()->OnPermissionsGrantedDynamicDelegate.AddDynamic(this, &UVirtDevice_Android::OnAndroidPermission);
	
	if (UAndroidPermissionFunctionLibrary::CheckPermission(m_fineLocationPermissionString))
	{
		InitBluetooth();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[CYBERITH] VirtDevice_Android : BLUETOOTH_CONNECT Android Permission not yet granted"));
	}
}

void UVirtDevice_Android::InitBluetooth()
{
	UE_LOG(LogTemp, Display, TEXT("[CYBERITH] VirtDevice_Android : Initializing Bluetooth"));
	AndroidVirtDeviceObject->CallMethod<void>(InitMethod);
	
	const FString SavedVirtSamName = FSavedVirtDataManager::GetSavedVirtSamName();
	if (SavedVirtSamName != "")
	{
		m_savedVirtSamName = SavedVirtSamName;
	}

	FindAndConnectSavedVirtSam();
}

TArray<FString> UVirtDevice_Android::GetPairedDevices()
{
	return m_pairedDevices;
}

FString UVirtDevice_Android::GetSavedVirtSamName()
{
	return m_savedVirtSamName;
}

void UVirtDevice_Android::SaveToFileVirtSam(FString Name)
{
	m_savedVirtSamName = Name;
	FSavedVirtDataManager::SaveToFile(m_savedVirtSamName);
}

void UVirtDevice_Android::FindAndConnectSavedVirtSam()
{
	auto JSavedVirtSamName = FJavaClassObject::GetJString(*m_savedVirtSamName);
	m_pairedDevices = JavaTypesConvertor::ToStringArray(AndroidVirtDeviceObject->CallMethod<jobjectArray>(FindPairedVirtSamAndConnectMethod, *JSavedVirtSamName));
	if (m_pairedDevices[0] == "false")
	{
		SetStateTo(Selecting);
	}
}

void UVirtDevice_Android::SetStateTo(EBleConnectionStates NewState)
{
	m_state = NewState;
	UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EBleConnectionStates"), true);
	FString StateStr = Enum->GetNameStringByIndex(NewState);
	UE_LOG(LogTemp, Display, TEXT("[CYBERITH] VirtDevice_Android : New BLE Connection State: %s"), *StateStr);

	if (NewState == Active)
	{
		if (!IsOpen())
		{
			Open();
		}
		VirtDeviceInfo.MajorVersion = AndroidVirtDeviceObject->CallMethod<int32>(GetMajorVersionMethod);
		VirtDeviceInfo.MinorVersion = AndroidVirtDeviceObject->CallMethod<int32>(GetMinorVersionMethod);;
		VirtDeviceInfo.ProductId = AndroidVirtDeviceObject->CallMethod<int32>(GetProductIdMethod);;
		VirtDeviceInfo.ProductName = AndroidVirtDeviceObject->CallMethod<FString>(GetProductNameMethod);
		VirtDeviceInfo.VendorId = AndroidVirtDeviceObject->CallMethod<int32>(GetVendorIdMethod);;
		VirtDeviceInfo.VendorName = AndroidVirtDeviceObject->CallMethod<FString>(GetVendorNameMethod);
		
		TSharedPtr<FVirtInputDevice> m_deviceController = FCybSDK_PluginModule::GetVirtualizerInputDevice();
		m_deviceController->OnBleConnectionActive();
	}
	
	if (OnBleStateChanged.IsBound())
		OnBleStateChanged.Broadcast(NewState);
}

void UVirtDevice_Android::BindToBleStateChangedDelegate(UObject* TargetObject, FName FuncName)
{
	FScriptDelegate d;
	d.BindUFunction(TargetObject, FuncName);
	OnBleStateChanged.Add(d);
}

void UVirtDevice_Android::SetOrientationOffset(float offset)
{
	m_orientationOffset = offset / 360.0f;
	UE_LOG(LogTemp, Display, TEXT("[CYBERITH] VirtDevice_Android : Orientation offset set to %f"), m_orientationOffset);
}

EBleConnectionStates UVirtDevice_Android::GetCurrentState()
{
	return m_state;
}

void UVirtDevice_Android::Tick(float DeltaTime)
{
	if (m_state == Active)
	{
		m_lastHapticTime += DeltaTime;
		if (m_gain != m_lastGain || m_frequency != m_lastFrequency || m_volume != m_lastVolume)
		{
			if (m_lastHapticTime >= m_hapticPeriod)
			{
				AndroidVirtDeviceObject->CallMethod<void>(HapticSetValues, m_gain, m_frequency, m_volume);
				m_lastHapticTime = 0;
				m_lastGain = m_gain;
				m_lastFrequency = m_frequency;
				m_lastVolume = m_volume;
			}
		}
	}
}

bool UVirtDevice_Android::Open()
{
	if (m_state >= Active)
	{
		return AndroidVirtDeviceObject->CallMethod<bool>(OpenMethod);
	}	
	return false;
}

bool UVirtDevice_Android::IsOpen() const {	return m_state == Active && AndroidVirtDeviceObject->CallMethod<bool>(IsOpenMethod); }

bool UVirtDevice_Android::Close()
{
	if (m_state >= Active)
	{
		return AndroidVirtDeviceObject->CallMethod<bool>(CloseMethod);
	}
	return false;
}

FVirtDeviceInfo_Android& UVirtDevice_Android::GetDeviceInfo() {	return VirtDeviceInfo; }

float UVirtDevice_Android::GetPlayerHeight() const { return static_cast<float>(AndroidVirtDeviceObject->CallMethod<int>(GetPlayerHeightMethod)) / 10000.0f; }

void UVirtDevice_Android::ResetPlayerHeight()
{
	if (m_state >= Active)
	{
		AndroidVirtDeviceObject->CallMethod<void>(ResetPlayerHeightMethod);
	}
}
float UVirtDevice_Android::GetPlayerOrientation() const
{
	//return static_cast<float>(AndroidVirtDeviceObject->CallMethod<int>(GetPlayerOrientationMethod)) / 10000.0f - m_orientationOffset / 360.0f;
	float PlayerOrientation = static_cast<float>(AndroidVirtDeviceObject->CallMethod<int>(GetPlayerOrientationMethod)) / 10000.0f;//GetPlayerOrientationWithoutOffset();
	PlayerOrientation -= m_orientationOffset;
	//UE_LOG(LogTemp, Display, TEXT("[CYBERITH] VirtDevice_Android : m_orientationOffset: %f"), m_orientationOffset);
	//UE_LOG(LogTemp, Display, TEXT("[CYBERITH] VirtDevice_Android : PlayerOrientation: %f"), PlayerOrientation);
	//PlayerOrientation = FMath::Fmod(PlayerOrientation, 1.0f);
	if (PlayerOrientation < 0)
	{
		PlayerOrientation += 1.0f;
	} else if (PlayerOrientation >= 1.0f )
	{
		PlayerOrientation -= 1.0f;
	}
	return PlayerOrientation;
}

void UVirtDevice_Android::ResetPlayerOrientation()
{
	if (m_state >= Active)
	{
		AndroidVirtDeviceObject->CallMethod<void>(ResetPlayerOrientationMethod);
	}
}

float UVirtDevice_Android::GetMovementSpeed() const {	return static_cast<float>(AndroidVirtDeviceObject->CallMethod<int>(GetMovementSpeedMethod)) / 10000.0f; }

float UVirtDevice_Android::GetMovementDirection() const {	return static_cast<float>(AndroidVirtDeviceObject->CallMethod<int>(GetMovementDirectionMethod)) / 10000.0f; }

bool UVirtDevice_Android::HasHaptic() const {	return AndroidVirtDeviceObject->CallMethod<bool>(HasHapticMethod); }

void UVirtDevice_Android::HapticPlay()
{
	if (m_state >= Active)
	{
		AndroidVirtDeviceObject->CallMethod<void>(HapticPlayMethod);
	}
}

void UVirtDevice_Android::HapticStop()
{
	if (m_state >= Active)
	{
		AndroidVirtDeviceObject->CallMethod<void>(HapticStopMethod);
	}
}

void UVirtDevice_Android::HapticSetGain(int gain) {	m_gain = gain; }

void UVirtDevice_Android::HapticSetFrequency(int frequency) {	m_frequency = frequency; }

void UVirtDevice_Android::HapticSetVolume(int volume) {	m_volume = volume; }

TStatId UVirtDevice_Android::GetStatId() const {	return TStatId(); }

JNI_METHOD void Java_com_cyberith_unrealengineble_AndroidVirtDevice_OnStateChange(JNIEnv* env, jclass clazz, jlong objAddr, jstring newStateStr)
{
	FString NewStatStr = FJavaHelper::FStringFromParam(env, newStateStr);
	UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EBleConnectionStates"), true);
	int32 StateIndex = Enum->GetIndexByName(FName(*NewStatStr));
	UVirtDevice_Android* Device = reinterpret_cast<UVirtDevice_Android*>(objAddr);
	AsyncTask(ENamedThreads::GameThread, [=]() {
		Device->SetStateTo(EBleConnectionStates(StateIndex));
	});
}

JNI_METHOD void Java_com_cyberith_unrealengineble_AndroidVirtDevice_OnAndroidError(JNIEnv* env, jclass clazz, jstring errorMessage)
{
	FString ErrorMessage = FJavaHelper::FStringFromParam(env, errorMessage);
	UE_LOG(LogTemp, Error, TEXT("[CYBERITH] Android :  %s"), *ErrorMessage);
}

JNI_METHOD void Java_com_cyberith_unrealengineble_AndroidVirtDevice_OnAndroidWarning(JNIEnv* env, jclass clazz, jstring warningMessage)
{
	FString WarningMessage = FJavaHelper::FStringFromParam(env, warningMessage);
	UE_LOG(LogTemp, Warning, TEXT("[CYBERITH] Android :  %s"), *WarningMessage);
}
