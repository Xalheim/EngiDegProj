#pragma once

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJava.h"

namespace JavaTypesConvertor
{
	TArray<FString> ToStringArray(jobjectArray JavaArray);
	TArray<uint8> ToByteArray(jbyteArray JavaArray);
};
