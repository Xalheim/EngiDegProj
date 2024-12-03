#include "JavaTypesConvertor.h"

namespace JavaTypesConvertor
{
	TArray<FString> ToStringArray(jobjectArray JavaArray)
	{
		JNIEnv* Env = FAndroidApplication::GetJavaEnv();
		const int ArrayLength = Env->GetArrayLength(JavaArray);

		TArray<FString> StringArray;
		for (int i = 0; i < ArrayLength; i++)
		{
			jstring JavaString = static_cast<jstring>(Env->GetObjectArrayElement(JavaArray, i));
			StringArray.Add(FJavaHelper::FStringFromLocalRef(Env, JavaString));
		}

		return StringArray;
	}

	TArray<uint8> ToByteArray(jbyteArray JavaArray)
	{
		JNIEnv* Env = FAndroidApplication::GetJavaEnv();
		const int DataLength = Env->GetArrayLength(JavaArray);
		jbyte* JavaBytes = Env->GetByteArrayElements(JavaArray, 0);

		TArray<uint8> ByteArray;
		for (int i = 0; i < DataLength; i++)
		{
			ByteArray.Add(JavaBytes[i]);
		}
		return ByteArray;
	}
}