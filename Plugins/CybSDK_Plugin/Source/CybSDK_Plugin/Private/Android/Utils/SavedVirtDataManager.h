#pragma once

using namespace std;

class FSavedVirtDataManager
{
public:
	static void SaveToFile(FString VirtSamName);
	static FString GetSavedVirtSamName();

private:
	static const FString FilePath;
};
