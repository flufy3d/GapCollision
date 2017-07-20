/*

	By Rama

*/
#include "GapCollisionPrivatePCH.h"
#include "GapLandscapeCollision.h"
#include "PhysicsEngine/BodySetup.h"
//////////////////////////////////////////////////////////////////////////


UGapLandscapeCollision::UGapLandscapeCollision(const class FObjectInitializer& PCIP)
	: Super( PCIP )
{
	PrimaryComponentTick.bCanEverTick = false;
	
}
 
bool UGapLandscapeCollision::SetGeneratedMeshTriangles(const TArray<FVMeshTri>& Triangles)
{
	GeneratedMeshTris.Empty();
	GeneratedMeshTris.Append(Triangles);
 
//#if WITH_EDITOR
	if (ModelBodySetup) ModelBodySetup->InvalidatePhysicsData(); // This is required for the first time after creation
//#endif
 
	UpdateCollision();
 
	// Need to recreate scene proxy to send it over
	MarkRenderStateDirty();

	return true;
}
 
 
FPrimitiveSceneProxy* UGapLandscapeCollision::CreateSceneProxy()
{
	//~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~
	//Dont create the scene proxy :)
	//		Used for collision only!
	return NULL;
	//~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~
}
 
int32 UGapLandscapeCollision::GetNumMaterials() const
{
	return 1;
}
 
FBoxSphereBounds UGapLandscapeCollision::CalcBounds(const FTransform & LocalToWorld) const
{
	TArray<FVector> Points;
	
	//Generate Box from All Vertices, transformed into World Space
	const int32 Total = GeneratedMeshTris.Num();
	for(int32 v= 0 ; v < Total; v++)
	{
		if( !GeneratedMeshTris.IsValidIndex(v)) return FBoxSphereBounds();
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		Points.Add(GeneratedMeshTris[v].Vertex0.Position);
		Points.Add(GeneratedMeshTris[v].Vertex1.Position);
		Points.Add(GeneratedMeshTris[v].Vertex2.Position);
	}
	return FBoxSphereBounds(FBox(Points)).TransformBy(LocalToWorld);
	
	//THE SIZE OF THE ENTIRE WORLD 
	/*
	FBoxSphereBounds NewBounds;
	NewBounds.Origin = FVector::ZeroVector;
	NewBounds.BoxExtent = FVector(HALF_WORLD_MAX,HALF_WORLD_MAX,HALF_WORLD_MAX);
	NewBounds.SphereRadius = FMath::Sqrt(3.0f * FMath::Square(HALF_WORLD_MAX));
	return NewBounds;
	*/
}
 
 
bool UGapLandscapeCollision::GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData)
{
	FTriIndices Triangle;
 
	for(int32 i=0;i<GeneratedMeshTris.Num();i++) {
		const FVMeshTri& tri = GeneratedMeshTris[i];
 
		Triangle.v0 = CollisionData->Vertices.Add(tri.Vertex0.Position);
		Triangle.v1 = CollisionData->Vertices.Add(tri.Vertex1.Position);
		Triangle.v2 = CollisionData->Vertices.Add(tri.Vertex2.Position);
 
		CollisionData->Indices.Add(Triangle);
		CollisionData->MaterialIndices.Add(i);
	}
 
	CollisionData->bFlipNormals = true;
 
	return true;
}
 
bool UGapLandscapeCollision::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
{
	return (GeneratedMeshTris.Num() > 0);
}
 
void UGapLandscapeCollision::UpdateBodySetup() {
	if (ModelBodySetup == NULL)	{
		ModelBodySetup = NewObject<UBodySetup>(this);
		ModelBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		ModelBodySetup->bMeshCollideAll = true;
	}
}
 
void UGapLandscapeCollision::UpdateCollision() {
	if (bPhysicsStateCreated) {
		DestroyPhysicsState();
		UpdateBodySetup();
		CreatePhysicsState();
 
//#if WITH_EDITOR
		ModelBodySetup->InvalidatePhysicsData();
//#endif
		ModelBodySetup->CreatePhysicsMeshes();
	}
}
 
UBodySetup* UGapLandscapeCollision::GetBodySetup() {
	UpdateBodySetup();
	return ModelBodySetup;
}


//~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~
//	Joy Init
//~~~~~~~~~~~~~~
void UGapLandscapeCollision::UpdateMeshFromArray(const TArray<FVMeshTri>& Triangles)
{
	SetGeneratedMeshTriangles(Triangles);
	UpdateBodySetup();
	UpdateCollision();
}