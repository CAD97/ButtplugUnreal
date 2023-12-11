// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

using System.IO;
using UnrealBuildTool;

public class Buttplug : ModuleRules
{
	public Buttplug(ReadOnlyTargetRules Target) : base(Target)
	{
        DefaultBuildSettings = BuildSettingsVersion.V2;
#if UE_5_2_OR_LATER
		DefaultBuildSettings = BuildSettingsVersion.V3;
#endif
#if UE_5_3_OR_LATER
		DefaultBuildSettings = BuildSettingsVersion.V4;
#endif

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
        });
		
		PrivateDependencyModuleNames.AddRange(new string[]
        {
            "CoreUObject",
            "Engine",
			"Json",
			"WebSockets",
		});

		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
	}
}
