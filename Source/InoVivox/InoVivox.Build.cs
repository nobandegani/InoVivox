// Copyright Inoland All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;

public class InoVivox : ModuleRules
{

	public InoVivox(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			PublicDefinitions.Add("DISABLE_VIVOXCORE=1");
		}
		else
		{
			PublicDefinitions.Add("DISABLE_VIVOXCORE=0");
		}
		
		#if !DISABLE_VIVOXCORE
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"VivoxCore"
			});
		#endif
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				//for core
				"Core",
				"CoreUObject",
				"Engine",	
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
		);
		
		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Public"),
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Private"),
				// ... add other private include paths required here ...
			}
			);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
	
	
}
