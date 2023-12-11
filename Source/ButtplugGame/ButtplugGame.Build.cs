// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

using UnrealBuildTool;

public class ButtplugGame : ModuleRules
{
	public ButtplugGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"InputCore",
		});
	}
}
