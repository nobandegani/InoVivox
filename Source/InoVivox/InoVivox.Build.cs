// Copyright Inoland All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class InoVivox : ModuleRules
{
	public InoVivox(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDefinitions.Add("DISABLE_VIVOXCORE=0");
		
		if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			PublicDefinitions.Add("DISABLE_VIVOXCORE=1");
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


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore"
				// ... add private dependencies that you statically link with here ...	
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
