// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class QuadLand : ModuleRules
{
	public QuadLand(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { "QuadLand" });
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG","ModularGameplay", "OnlineSubsystemSteam", "OnlineSubsystem"  });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "GameFeatures",
            "GameplayAbilities",
            "GameplayTasks",
            "GameplayTags",
            "ModularGameplay"
        }) ;
    }
}
