// Copyright Epic Games, Inc. All Rights Reserved.
using System;
//using System.Diagnostics.Metrics;
using System.IO;

namespace UnrealBuildTool.Rules
{
	public class GateXRMovement : ModuleRules
	{
		public GateXRMovement(ReadOnlyTargetRules Target) : base(Target)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"ApplicationCore",
					"Engine",
					"InputCore",
					"HeadMountedDisplay",
					"OVRPluginXR",
					"OculusXRHMD",
					"OculusXRMovement"
				});

			// PublicDependencyModuleNames.AddRange(
			// 	new string[]
			// 	{
			// 		"OculusXRHMD",
			// 		"OculusXRMovement"
			// 	});

			// DONOT use PoseableMesh for EyeTracking 
			PrivateDefinitions.Add("WITH_EYETRACKING_POSEABLEMESH=0");


            var EngineDir = Path.GetFullPath(Target.RelativeEnginePath); // if MetaXR is inside Marketplace 
			var PluginDir = Path.Combine(Path.GetDirectoryName(Path.GetDirectoryName(ModuleDirectory)), "Plugins");

            PrivateIncludePaths.AddRange(
				new string[] {
                    // Relative to Engine\Plugins\Runtime\OculusXR\OculusXRVR\Source
					// 1) if MetaXR is inside Marketplace 
                    Path.Combine(EngineDir, "Plugins/Marketplace/MetaXR/Source/OculusXRHMD/Private"),
                    Path.Combine(EngineDir, "Plugins/Marketplace/MetaXR/Source/OculusXRMovement/Private")
					// 2) if MetaXR is inside Game/Plugins
					// Path.Combine(PluginDir, "MetaXR/Source/OculusXRHMD/Private"),
                    // Path.Combine(PluginDir, "MetaXR/Source/OculusXRMovement/Private")
					// "OculusXRHMD/Private"
				}) ;
			
			PublicIncludePaths.AddRange(
				new string[] {
					// 1) if MetaXR is inside Marketplace 
					Path.Combine(EngineDir, "Plugins/Marketplace/MetaXR/Source/OculusXRMovement/Public")
					// 2) if MetaXR is inside Game/Plugins
					// Path.Combine(PluginDir, "MetaXR/Source/OculusXRMovement/Public")
					// "OculusXRMovement/Public"
				}
			);
		}
	}
}
