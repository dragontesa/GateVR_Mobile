// Copyright Epic Games, Inc. All Rights Reserved.
using System;
using System.IO;

namespace UnrealBuildTool.Rules
{
	public class GateGameLift : ModuleRules
	{
		public GateGameLift(ReadOnlyTargetRules Target) : base(Target)
		{
			PublicDefinitions.Add("GAMELIFT_SERVER_SDK_VER=4");

            if (Target.Type == TargetRules.TargetType.Server)
		    {
		    	PublicDefinitions.Add("WITH_GAMELIFT=1");
		    }
		    else
		    {
		    	PublicDefinitions.Add("WITH_GAMELIFT=0");
		    }

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"ApplicationCore",
					"Engine",
					"InputCore"
				});

			PublicDependencyModuleNames.AddRange(new string[] { 
				"AWSCore", "AWSCoreLibrary", "GameLiftServerSDK", "GameLift"
			});

		}
	}
}
