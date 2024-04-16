// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class GateVREditorTarget : TargetRules
{
	public GateVREditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
        // bUsesSteam = true;

        bool USE_AWSSDK_PLUGIN = true;
        if (!USE_AWSSDK_PLUGIN)
        {
            DisablePlugins.Add("awsSDK");
        }

        // ExtraModuleNames.AddRange( new string[] { "GateVR", "GateXRMovement", "GateGameLift", "GatePixelStreaming"} );
        ExtraModuleNames.AddRange( new string[] { "GateVR", "GateXRMovement"} );
	}
}
