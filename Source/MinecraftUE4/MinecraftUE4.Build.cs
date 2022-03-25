// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MinecraftUE4 : ModuleRules
{
    public MinecraftUE4(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "SimplexNoise" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ProceduralMeshComponent" });
    }
}
