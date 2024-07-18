// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;

public class RuntimeGlobalConfigure : ModuleRules
{
	public RuntimeGlobalConfigure(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        string SourceDir = System.IO.Path.Combine(ModuleDirectory, "..\\");

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
                Path.Combine(ModuleDirectory, SourceDir + "/ThirdParty/YamlCPP/include"),
            }
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "Json",
                "JsonUtilities",
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        PublicAdditionalLibraries.AddRange(
			new string[]
			{
                Path.Combine(SourceDir + "/ThirdParty/YamlCPP/lib", "yaml-cpp.lib"),
            }
			);
    }
}