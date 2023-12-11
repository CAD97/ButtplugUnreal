// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

using UnrealBuildTool;

public class ButtplugGameEditorTarget : TargetRules
{
	public ButtplugGameEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
	}
}
