#include "SavedVirtDataManager.h"

#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

const FString FSavedVirtDataManager::FilePath = "cyberith_data.dat";

void FSavedVirtDataManager::SaveToFile(FString VirtSamName)
{
	FString file = FPaths::ProjectConfigDir();
	file.Append(FilePath);

	FFileHelper::SaveStringToFile(VirtSamName,*file);
}

FString FSavedVirtDataManager::GetSavedVirtSamName()
{
	FString file = FPaths::ProjectConfigDir();
	file.Append(FilePath);
	
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	FString VirtSamName = "";
	if (FileManager.FileExists(*file))
	{
		FFileHelper::LoadFileToString(VirtSamName,*file,FFileHelper::EHashOptions::None);
	}

	return VirtSamName;
}
