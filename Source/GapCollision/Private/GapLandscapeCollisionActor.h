#pragma once

//Gap Landscape Collision Component
#include "GapLandscapeCollision.h"

//Landscape Data
#include "Landscape.h"
#include "LandscapeComponent.h"
#include "LandscapeDataAccess.h"
#include "LandscapeLayerInfoObject.h"
//Base
#include "GapLandscapeCollisionActor.generated.h"

//~~~~~~~~~~~~~~
//Custom Subdivisions
#define VICTORY_LANDSCAPE_COLLISION_MAX 144  //673 for visuals
//~~~~~~~~~~~~~~

UCLASS()
class AGapLandscapeCollisionActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
	/** Initial Load Timer Speed. Higher numbers means slower and higher frame rate. Lower numbers means faster but more chance of FPS drops for a moment or 2. Recommended lowest = 0.01 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gap Landscape")
	float GapLandscape_LoadTimerSpeed;
	
	/** Component Load Timer Speed. Higher numbers means slower and higher frame rate. Lower numbers means faster but more chance of FPS drops for a moment or 2. Recommended lowest = 0.01 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gap Landscape")
	float GapLandscape_ComponentTimerSpeed;
	
	/** Replace Landscape collision, or only patch it? If replacing, make sure to turn of the Landscape Actor's Collision in the Editor to save resources. If patching, triangles will only be created where there is a steep enough slope, to minimize triangle count. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gap Landscape")
	bool GapLandscape_DoReplace;
	
	/** If not replacing, and only Patching Landscape collision, this is the worldspace max z difference in triangle vertex positions that is minimum required for the triangle to count and be created. The bigger this number the steeper the slopes have to be for the triangles to be included */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gap Landscape")
	float GapLandscape_RequiredZDifferenceToAddTriangle;
	
	/** The height above/belove the Landscape Actor for the Gap Landscape Collision Mesh! */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gap Landscape")
	float GapLandscape_HeightOffsetForCollisionMesh;
	
public:
	//Material for Testing
	UPROPERTY()
	UMaterialInterface* Material;
	
	//Root Comp
	UPROPERTY()
		UGapLandscapeCollision* LandscapeCollisionRoot;
	
	//Components
	UPROPERTY()
	TArray< UGapLandscapeCollision* > CollisionComponents;
	
	//Core Data
	UPROPERTY()
	TArray<FVMeshTri> LandscapeTriangles;
	
//~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Custom Component Subdivisions
//		Dividing up the triangle collision 
//			into desired number of sub components!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~
public:
	UPROPERTY()
		UGapLandscapeCollision* LandscapeCollision[VICTORY_LANDSCAPE_COLLISION_MAX];

	//Spreads out the LandscapeTriangles among the total custom amount of collision components!
	//		SHOULD RESEARCH WHAT IS THE SIMPLEST COLLISION COMPONENT
	//			WHEN NOT TESTING STUFF AND NEEDING VISIUAL
	void CreateLandscapeCollision();
	
//~~~~~~~~~~~~~~~~~~~~~~~~
// Triangle Max Height Difference Test
//		by Rama
//~~~~~~~~~~~~~~~~~~~~~~~~
public:
	FORCEINLINE float GetTriangleMaxHeightDifference(const FVMeshTri& Tri)
	{
		const float Heights[3] =
		{
			FMath::Abs(Tri.Vertex0.Position.Z - Tri.Vertex1.Position.Z),
			FMath::Abs(Tri.Vertex0.Position.Z - Tri.Vertex2.Position.Z),
			FMath::Abs(Tri.Vertex1.Position.Z - Tri.Vertex2.Position.Z)
		};
		return FMath::Max(  FMath::Max(Heights[0],Heights[1]),  Heights[2]);
	}
//~~~~~~~~~~~~~~~~~~~~~~~
//					  Joy Init
//~~~~~~~~~~~~~~~~~~~~~~~
public:
	
	void JoyInit(ALandscape* IN_LandscapeActor);
	void JoyInit(ALandscape* IN_LandscapeActor, UMaterialInterface* IN_Material);
	
	//The Landscape Component to make triangles for
	//		Stored in Global Array LandscapeTriangles
	void CreateTrianglesForComp(ULandscapeComponent* LandscapeComp);

public:
	FORCEINLINE void VSetupComponent(UPrimitiveComponent* Comp)
	{
		if(!Comp) return;
		//~~~~~~~~
		
		Comp->AlwaysLoadOnClient = true;
		Comp->AlwaysLoadOnServer = true;
		
		//Collision
		Comp->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Comp->BodyInstance.SetResponseToAllChannels(ECR_Block);
		Comp->BodyInstance.SetResponseToChannel(ECC_WorldStatic, ECR_Block);
		Comp->BodyInstance.SetResponseToChannel(ECC_WorldDynamic, ECR_Block);
		Comp->BodyInstance.SetResponseToChannel(ECC_Pawn, ECR_Block);
	
		//Movement
		Comp->BodyInstance.SetObjectType(ECC_WorldDynamic);
		
		//Special Way to Get Physics Pieces to interact with the Component
		LandscapeCollisionRoot->SetMobility(EComponentMobility::Stationary);
		
		//Visibility
		Comp->CastShadow = false;
		Comp->SetHiddenInGame(true); //hide! 
	}

protected:
	
	/** Tick! */
	virtual void Tick(float DeltaTime) override;
	
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//			Process One Component Per Tick

	ALandscape* LandscapeActor;
	uint32 CurLandscapeComponent;
	void TickProcess_ProcessTriangles();


	uint32 Tick_Process_TrianglesPerComponent;
	uint32 TickProcess_CurComp;
	uint32 TickProcess_CurTriIndex;
	void TickProcess_ProcessComponents();


	UPROPERTY()
	FTimerHandle ProcessTrianglesHandle;

	UPROPERTY()
	FTimerHandle ProcessComponentsHandle;

};