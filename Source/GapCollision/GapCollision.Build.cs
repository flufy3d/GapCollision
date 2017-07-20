// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class GapCollision : ModuleRules
    {
        public GapCollision(TargetInfo Target)
        {
            //Private Paths
            PrivateIncludePaths.AddRange(new string[] {
            "GapCollision/Private"
        });

            PublicDependencyModuleNames.AddRange(
                new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "Landscape",
                "InputCore",
                "RHI",
                "ProceduralMeshComponent"
                }
            );
        }
    }
}
