// Copyright Epic Games, Inc. All Rights Reserved.
using System;
using System.IO;

namespace UnrealBuildTool.Rules
{
	public class GatePixelStreaming : ModuleRules
	{
		public GatePixelStreaming(ReadOnlyTargetRules Target) : base(Target)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"ApplicationCore",
					"Engine",
					"InputCore",
                    "PixelStreaming" 
				});

                var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
         		PrivateIncludePaths.AddRange(
				new string[] {
					Path.Combine(EngineDir, "Plugins/Media/PixelStreaming/Source/PixelStreaming/Public"),
				});

		}
	}
}
