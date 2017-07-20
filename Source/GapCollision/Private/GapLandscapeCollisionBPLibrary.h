// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GapLandscapeCollisionBPLibrary.generated.h"

// BP Library for You
//
// Written by Rama

UCLASS()
class GAPCOLLISION_API UGapLandscapeCollisionBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
	
	/** Create Gap Landscape Collision for the Passed in Landscape Actor! If ReplaceLandscapeCollision is false, only patching will occur in areas of steep slope. If true, replaces all collision. Make sure to turn of the Landscape Actor's collision to conserve resources! Use higher timer speeds if getting FPS stuttering when this node runs. Returns false if the operation could not occur. */
	UFUNCTION(BlueprintCallable, Category = "GapLandscapeCollisionLibrary", meta=(HidePin="WorldContextObject", DefaultToSelf="WorldContextObject" ) )
	static bool GapLandscapeCollision__CreateCollisionForLandscape(
		UObject* WorldContextObject, 
		ALandscape* LandscapeActor,
		bool ReplaceLandscapeCollision = false,
		float RequiredSlopeVarianceToAddTriangle = 33,
		float CollisionMeshHeightOffset = 1,
		float TriangleTimerSpeed = 0.01,
		float ComponentTimerSpeed = 0.01
	);
	
};

