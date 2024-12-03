// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class CybSDK_Plugin : ModuleRules
{
	public CybSDK_Plugin(ReadOnlyTargetRules Target) : base(Target)
	{
#if UE_4_21_OR_LATER
		PrivatePCHHeaderFile = "Public/CybSDK_PluginPCH.h";
#endif

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				// UE4 Doku: This setting is currently not need as we discover all files from the 'Public' folder
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				"CybSDK_Plugin/Public",
				//"CybSDK_Plugin/Public/Android",
				"CybSDK_Plugin/Public/Components",
				"CybSDK_Plugin/Public/CybSDK",
				"CybSDK_Plugin/Public/Input",
				"CybSDK_Plugin/Public/Standalone"
				//"ThirdParty/CybSDK",
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Projects",
				"Engine",
				"InputCore",
				"InputDevice",
				"AIModule",
				"AndroidPermission", 
				"UMG",
#if !UE_5_0_OR_LATER
				"GeometricObjects"
#endif
#if UE_5_3_OR_LATER
				"XRBase"
#endif
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"HeadMountedDisplay"
				//"Launch"
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		if (Target.bBuildEditor == true)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"UnrealEd"
				}
			);
		}

		if (Target.Platform == UnrealTargetPlatform.Win64 /*|| Target.Platform == UnrealTargetPlatform.Win32*/)
		{
			PrivateIncludePaths.Add("ThirdParty/CybSDK");
			// Plugin name
			string PluginName = "CybSDK_Plugin";

			// Platform (Win32 / Win64)
			string TargetPlatformStr = Target.Platform.ToString();

			// Directory - Input
			string PluginBaseDirectory = Path.Combine(ModuleDirectory, "..", "..");
			// Directory - Output
			string OutputDirectory = Path.Combine("$(TargetOutputDir)", "..", "..", "Plugins", PluginName);

			// CybSDK source (not Unreal-related, Source, Lib, dlls
			string CybSDKSourceDirectory = Path.Combine(PluginBaseDirectory, "Source", "ThirdParty", "CybSDK");
			string CybSDKLibDirectory = Path.Combine(CybSDKSourceDirectory, TargetPlatformStr);
			string CybSDKBinaryPath = Path.Combine(PluginBaseDirectory, "Binaries", "ThirdParty", "CybSDK", TargetPlatformStr);
			string CybSDKDllPath = Path.Combine(CybSDKBinaryPath, "CybSDK.dll");

			// Directories - Output
			string OutputCybSDKBinaryPath = Path.Combine(OutputDirectory, "Binaries", "ThirdParty", "CybSDK", TargetPlatformStr);
			string OutputCybSDKDllPath = Path.Combine(OutputCybSDKBinaryPath, "CybSDK.dll");

			// Include path to the .h
			PublicIncludePaths.Add(CybSDKSourceDirectory);
			PublicSystemIncludePaths.Add(CybSDKSourceDirectory);

			// Lib
// #if UE_4_21_OR_LATER
			PublicAdditionalLibraries.Add(Path.Combine(CybSDKLibDirectory, "CybSDK.lib"));
// #else
//			PublicLibraryPaths.Add(CybSDKLibDirectory); 	// in 4.25, 4.26 and later, we should use PublicAdditionalLibraries instead
// #endif

			// DLL - Load
			PublicDelayLoadDLLs.Add("CybSDK.dll");
			
			// DLL Auto-copying
// #if UE_4_21_OR_LATER
			RuntimeDependencies.Add(Path.Combine(CybSDKBinaryPath, "CybSDK.dll"));
// #else
//			RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(CybSDKBinaryPath, "CybSDK.dll")));	// in 4.25, 4.26 and later, we should use PublicAdditionalLibraries instead
// #endif
		}
		else if (Target.Platform == UnrealTargetPlatform.Android)
		{
			//PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Standalone"));
			PrivateDependencyModuleNames.Add("Launch");

			string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "CybSDK_Android_UPL.xml"));
		}
	}
}
