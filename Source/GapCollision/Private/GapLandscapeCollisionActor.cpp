/*

	GapLandscapeCollisionActor

	by Rama
*/
#include "GapCollisionPrivatePCH.h"
#include "GapLandscapeCollisionActor.h"

AGapLandscapeCollisionActor::AGapLandscapeCollisionActor(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{	
	//Timers
	GapLandscape_LoadTimerSpeed 				= 0.01; //0.1 for visuals
	GapLandscape_ComponentTimerSpeed			= 0.01;
	
	GapLandscape_DoReplace 							= false;  //replace/patch
	GapLandscape_HeightOffsetForCollisionMesh 	= 1; 		//14 = prettier visualization
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	GapLandscape_RequiredZDifferenceToAddTriangle = 33;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	//Create
	LandscapeCollisionRoot = PCIP.CreateDefaultSubobject<UGapLandscapeCollision>(this, TEXT("LandscapeCollisionRoot"));
	
	//THIS IS THE ROOT 
	RootComponent = LandscapeCollisionRoot;
	
	//Setup Component!
	VSetupComponent(LandscapeCollisionRoot);
	
	//~~~
	
	for(uint32 v = 0; v < VICTORY_LANDSCAPE_COLLISION_MAX; v++)
	{
		const FString CompName = "GapLandscapeCollisionComp" + FString::FromInt(v);
		
		//Create
		LandscapeCollision[v] = PCIP.CreateDefaultSubobject<UGapLandscapeCollision>(this, *CompName);
		
		//Attach!
		LandscapeCollision[v]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		
		//Setup Component!
		VSetupComponent(LandscapeCollision[v]);
	}
	
	
	//tick
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
}

void AGapLandscapeCollisionActor::JoyInit(ALandscape* IN_LandscapeActor)
{
	if(!IN_LandscapeActor || !IN_LandscapeActor->IsValidLowLevel())
	{
		return;
	}
	
	LandscapeActor = IN_LandscapeActor;
	
	//Empty any previous triangles in case JoyInit is run multiple times!
	LandscapeTriangles.Empty();
	//~~~~~~~~~~~~~~~~
	
	//MAKE THIS INTO A / TASK GRAPH
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//								Process Triangles
	CurLandscapeComponent = 0;
	GetWorldTimerManager().SetTimer(ProcessTrianglesHandle,this,
		&AGapLandscapeCollisionActor::TickProcess_ProcessTriangles, GapLandscape_LoadTimerSpeed, true);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}
void AGapLandscapeCollisionActor::JoyInit(ALandscape* IN_LandscapeActor, UMaterialInterface* IN_Material)
{
	if(!IN_LandscapeActor || !IN_LandscapeActor->IsValidLowLevel())
	{
		return;
	}
	
	LandscapeActor = IN_LandscapeActor;
	
	//Empty any previous triangles in case JoyInit is run multiple times!
	LandscapeTriangles.Empty();
	//~~~~~~~~~~~~~~~~
	
	Material = IN_Material; //remember there's no UVs being created here (unless u choose to make based on the quads)
	
	//Main Component
	LandscapeCollisionRoot->SetMaterial(0,Material);
	
	//Custom Number of Components
	
	for(uint32 v = 0; v < VICTORY_LANDSCAPE_COLLISION_MAX; v++)
	{
		LandscapeCollision[v]->SetMaterial(0,Material);
	}
	
	
	
	//MAKE THIS INTO A / TASK GRAPH
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//								Process Triangles
	CurLandscapeComponent = 0;
	GetWorldTimerManager().SetTimer(ProcessTrianglesHandle,this,
		&AGapLandscapeCollisionActor::TickProcess_ProcessTriangles, GapLandscape_LoadTimerSpeed, true);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}

void AGapLandscapeCollisionActor::TickProcess_ProcessTriangles()
{
	
	
	//End Case
	if(CurLandscapeComponent >= uint32(LandscapeActor->LandscapeComponents.Num() ))
	{
		GetWorldTimerManager().ClearTimer(ProcessTrianglesHandle);
			
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//				Spread out the Gathered Triangles 
		//			among the custom number of components!
		CreateLandscapeCollision();
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		
		return;
	}
	
	//Valid Index
	if(!LandscapeActor->LandscapeComponents.IsValidIndex(CurLandscapeComponent))
	{
		GetWorldTimerManager().ClearTimer(ProcessTrianglesHandle);
			
		UE_LOG(Gap, Warning,TEXT("TickProcess_ProcessTriangles() Error Code 2"));
		return;
	}
	
	//Valid Component?
	ULandscapeComponent* EachComp = LandscapeActor->LandscapeComponents[CurLandscapeComponent];
	if(EachComp && EachComp->IsValidLowLevel())
	{
		CreateTrianglesForComp(EachComp);
	}
	
	
	//~~~~~~~~~~~~~
	//Inc!
	CurLandscapeComponent++;
	
}


void AGapLandscapeCollisionActor::CreateTrianglesForComp(ULandscapeComponent* LandscapeComp)
{
	//Get Landscape Data!
	FLandscapeComponentDataInterface LandscapeData(LandscapeComp, 0); //LOD 0
	
	//Lift the Additional Collision Slightly above the Landscape's Collision!
	const FVector GapHeightOffset = FVector(0,0,GapLandscape_HeightOffsetForCollisionMesh) - GetActorLocation();  //have to move into local space 
	
	//~~~
	
	//Testing Less than All
	
	/*
	//const int32 MaxIndex = LandscapeComp->ComponentSizeQuads+1;
	
	//for testing triangle creation
	// < 333 max so can see how the actual triangle verts line up
	const int32 Max = 33;
	int32 MaxIndex = Max;
	
	if(LandscapeComp->ComponentSizeQuads+1 < Max) 
		MaxIndex = LandscapeComp->ComponentSizeQuads+1;
	*/
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//								 Max
	
	//THE ACTUAL MAX IS ComponentSizeQuads+1
	//		so can go all the way to ComponentSizeQuads
	//			for the making of Quads which rely on the next index as well
	const int32 Max = LandscapeComp->ComponentSizeQuads;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	TArray<uint8> Weightmap;
	// Weight map
	FName target = FName(TEXT("guoling"));
	bool found = false;
	for (int32 i = 0; i < LandscapeComp->WeightmapLayerAllocations.Num(); i++)
	{

		FName name = LandscapeComp->WeightmapLayerAllocations[i].LayerInfo->LayerName;
		found = name.IsEqual(target);
		/*
		if (LandscapeData.GetWeightmapTextureData(LandscapeComp->WeightmapLayerAllocations[i].LayerInfo, Weightmap))
		{
			UE_LOG(LogTemp, Warning, TEXT("LayerInfo=%s "), *LandscapeComp->WeightmapLayerAllocations[i].LayerInfo->GetPathName());

		}
		*/
	}
	if (!found) return;


	TArray<FVMeshTri> Triangles;
	
	for(int32 Row = 0; Row < Max; Row++)
	{
		
		for(int32 Col = 0; Col < Max; Col++)
		{
			//~~~ Row and Col Values to Create 1 Quad (2 tri) ~~~
			
			//~~~ Triangle 1 ~~~
			
			//Create the Triangle and test whether it should be added!
			FVMeshTri NewTri1 =
				FVMeshTri(
						FDynamicMeshVertex(LandscapeData.GetWorldVertex(Col,		Row) 		+ GapHeightOffset),	
						FDynamicMeshVertex(LandscapeData.GetWorldVertex(Col,		Row + 1) 	+ GapHeightOffset),
						FDynamicMeshVertex(LandscapeData.GetWorldVertex(Col + 1,	Row) 		+ GapHeightOffset)
				);
					
			//Test whether to Add
			if(GapLandscape_DoReplace)
			{
				//Add!
				Triangles.Add(NewTri1);
			}
			else if(GetTriangleMaxHeightDifference(NewTri1) > GapLandscape_RequiredZDifferenceToAddTriangle)
			{
				//Add!
				Triangles.Add(NewTri1);
			}
			
			

			//~~~ Triangle 2 ~~~
			
			FVMeshTri NewTri2 =
				FVMeshTri(
						FDynamicMeshVertex(LandscapeData.GetWorldVertex(Col + 1,	Row + 1) 	+ GapHeightOffset),
						FDynamicMeshVertex(LandscapeData.GetWorldVertex(Col + 1,	Row) 		+ GapHeightOffset),
						FDynamicMeshVertex(LandscapeData.GetWorldVertex(Col,		Row + 1) 	+ GapHeightOffset)
				);
					 
			//Test whether to Add
			if(GapLandscape_DoReplace)
			{
				//Add!
				Triangles.Add(NewTri2);
			}
			else if(GetTriangleMaxHeightDifference(NewTri2) > GapLandscape_RequiredZDifferenceToAddTriangle)
			{
				//Add!
				Triangles.Add(NewTri2);
			}
		}
	}

	//~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~
	//		  Add to All Triangles
	LandscapeTriangles.Append(Triangles);
	//~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~
}

void AGapLandscapeCollisionActor::CreateLandscapeCollision()
{
	//Triangles total is less than component total?
	if(LandscapeTriangles.Num() < int32(VICTORY_LANDSCAPE_COLLISION_MAX) )
	{
		//If there are less Triangles then available components, just assign all to the root and exit
		
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//		 GENERATE MESH AND COLLISION 
		LandscapeCollisionRoot->UpdateMeshFromArray(LandscapeTriangles);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		return;
	}
	
	//~~~
	
	//Triangles Per Component
	Tick_Process_TrianglesPerComponent = LandscapeTriangles.Num()/VICTORY_LANDSCAPE_COLLISION_MAX;
	
	//Determine bunches of triangles
	UE_LOG(Gap, Warning,TEXT("Total Triangles/NumComponents %d"),Tick_Process_TrianglesPerComponent );
	
	//~~~
	
	// Tick Process
	
	//Dividing this up per tick allows for no hang during creation dynamic meshes!
	TickProcess_CurTriIndex = 0; //Tick_Process_TrianglesPerComponent - 1;
	TickProcess_CurComp = 0;
	GetWorldTimerManager().SetTimer(ProcessComponentsHandle,this,
		&AGapLandscapeCollisionActor::TickProcess_ProcessComponents, GapLandscape_ComponentTimerSpeed, true);
	
}

void AGapLandscapeCollisionActor::TickProcess_ProcessComponents()
{
	//End Case
	if(TickProcess_CurComp >= VICTORY_LANDSCAPE_COLLISION_MAX )
	{
		GetWorldTimerManager().ClearTimer(ProcessComponentsHandle);
		return;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}
	
	
	//GET BUNCH OF TRIANGLES FOR THIS COMPONENT
	TArray<FVMeshTri> TrianglesForComp;
	
	//Tris to get for this component from the Landscape Tri Array
	uint32 CurrentBunchMax = 0;
	
	//Is this the last comp?
	//		then add all remaining!
	if(TickProcess_CurComp == VICTORY_LANDSCAPE_COLLISION_MAX - 1 )
	{
		CurrentBunchMax = uint32(LandscapeTriangles.Num());
	}
	else
	{
		CurrentBunchMax = TickProcess_CurTriIndex + Tick_Process_TrianglesPerComponent;
	}
	
	//Invalid Index somehow?
	if( ! LandscapeTriangles.IsValidIndex(CurrentBunchMax - 1))
	{
		UE_LOG(Gap, Error,TEXT("AGapLandscapeCollisionActor::TickProcess_ProcessComponents()>> Error code 1 "));
		
		GetWorldTimerManager().ClearTimer(ProcessComponentsHandle);
		return;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}
	
	//Add the Triangles!
	for( ; TickProcess_CurTriIndex < CurrentBunchMax; TickProcess_CurTriIndex++ )
	{
		TrianglesForComp.Add(LandscapeTriangles[TickProcess_CurTriIndex]);
	}
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//		 GENERATE MESH AND COLLISION 
	LandscapeCollision[TickProcess_CurComp]->UpdateMeshFromArray(TrianglesForComp);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	//Inc!
	TickProcess_CurComp++;
}

void AGapLandscapeCollisionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//~~~~~~~~~~~~~
	
	//Using timer
}