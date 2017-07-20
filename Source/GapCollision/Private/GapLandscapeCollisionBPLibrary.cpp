/*

	By Rama

*/

#include "GapCollisionPrivatePCH.h"
#include "GapLandscapeCollisionBPLibrary.h"

#include "GapLandscapeCollisionActor.h"

//////////////////////////////////////////////////////////////////////////
// UGapLandscapeCollisionBPLibrary

UGapLandscapeCollisionBPLibrary::UGapLandscapeCollisionBPLibrary(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	
}

bool UGapLandscapeCollisionBPLibrary::GapLandscapeCollision__CreateCollisionForLandscape(
	UObject* WorldContextObject, 
	ALandscape* LandscapeActor,
	bool ReplaceLandscapeCollision,
	float RequiredSlopeVarianceToAddTriangle,
	float CollisionMeshHeightOffset,
	float TriangleTimerSpeed,
	float ComponentTimerSpeed
){
	if(!LandscapeActor) 		return false;
	if(!WorldContextObject) return false;
	//~~~~~~~~~~~~~~~~~~~~
	
	//using a context object to get the world!
    UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject);
	if(!World) return false;
	//~~~~~~~~~~~

	
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //guarantee spawn
	SpawnInfo.bDeferConstruction 	= false;
		
	//Spawn
	AGapLandscapeCollisionActor* LandscapeCollisionActor = 
		World->SpawnActor<AGapLandscapeCollisionActor>(FVector(0,0,1024),FRotator::ZeroRotator,SpawnInfo);
	
	if(!LandscapeCollisionActor) return false;
	//~~~~~~~~~~~~~~~~~~~~~~~~
	
	//UPROPERTY() VALUES
	LandscapeCollisionActor->GapLandscape_DoReplace 
		= ReplaceLandscapeCollision;
		
	LandscapeCollisionActor->GapLandscape_RequiredZDifferenceToAddTriangle 
		= RequiredSlopeVarianceToAddTriangle;
		
	LandscapeCollisionActor->GapLandscape_HeightOffsetForCollisionMesh
		= CollisionMeshHeightOffset;
		
	LandscapeCollisionActor->GapLandscape_LoadTimerSpeed 
		= TriangleTimerSpeed;
		
	LandscapeCollisionActor->GapLandscape_ComponentTimerSpeed 
		= ComponentTimerSpeed;
	
	//Joy Init Landscape Collision!
	LandscapeCollisionActor->JoyInit(LandscapeActor); 
	
	
	return true;
}