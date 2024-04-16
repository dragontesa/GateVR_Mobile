// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class GateVR : ModuleRules
{
	public GateVR(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", 
			"UMG", "Networking", "Sockets", "Json","Voice", 
		    });

		if (Target.Platform == UnrealTargetPlatform.Win64) {
			PublicDependencyModuleNames.AddRange(new string[] {
				 "HeadMountedDisplay"
			});
		}

        if (Target.Platform == UnrealTargetPlatform.Android || Target.Platform == UnrealTargetPlatform.Win64) 
		{
			// PublicDependencyModuleNames.AddRange(new string[] { "GateTalkingfree" });
			// PublicDependencyModuleNames.AddRange(new string[] {
			// 	 "OnlineSubsystem", "OnlineSubsystemNull", "OnlineSubsystemUtils"
			// });
			// DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		}

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore"});
        DynamicallyLoadedModuleNames.AddRange(new string[] { "InMemoryNetworkReplayStreaming" });

        // Hot fix 3dots screen
		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			// Solvation for android-33
			// var extraManifestFile = Path.Combine(ModuleDirectory, "ExtraPermissionAndroidManifest.xml");
			// AdditionalPropertiesForReceipt.Add("AndroidPlugin", extraManifestFile);
		}
        

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
