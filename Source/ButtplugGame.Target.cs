// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

using UnrealBuildTool;

public class ButtplugGameTarget : TargetRules
{
	public ButtplugGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
	}
}
