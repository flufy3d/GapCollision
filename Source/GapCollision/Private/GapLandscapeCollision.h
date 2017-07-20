/*

	Gap Landscape Collision

	by Rama
*/
#pragma once
//Base
#include "GapLandscapeCollision.generated.h"

//~~~ Gap Mesh Triangle ~~~
USTRUCT()
struct FVMeshTri
{
	GENERATED_USTRUCT_BODY()
 
	
	FDynamicMeshVertex Vertex0;
	FDynamicMeshVertex Vertex1;
	FDynamicMeshVertex Vertex2;
	
	FVMeshTri()
	{
	}
	
	FVMeshTri(const FDynamicMeshVertex& v1, const FDynamicMeshVertex& v2, const FDynamicMeshVertex& v3)
		: Vertex0(v1)
		, Vertex1(v2)
		, Vertex2(v3)
	{}
};

UCLASS(editinlinenew, meta=(BlueprintSpawnableComponent), ClassGroup=Rendering)
class UGapLandscapeCollision : public UMeshComponent, public IInterface_CollisionDataProvider 
{
	GENERATED_UCLASS_BODY()
 

//Joy Init
public:
	virtual void UpdateMeshFromArray(const TArray<FVMeshTri>& Triangles);
		
//~~~~
	
public:
	/** Set the geometry to use on this triangle mesh */
	UFUNCTION(BlueprintCallable, Category="Components|GeneratedMesh")
	bool SetGeneratedMeshTriangles(const TArray<FVMeshTri>& Triangles);
 
	/** Description of collision */
	UPROPERTY(BlueprintReadOnly, Category="Collision")
	class UBodySetup* ModelBodySetup;
 
	// Begin UMeshComponent interface.
	virtual int32 GetNumMaterials() const override;
	// End UMeshComponent interface.
 
	// Begin Interface_CollisionDataProvider Interface
	virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	virtual bool WantsNegXTriMesh() override { return false; }
	// End Interface_CollisionDataProvider Interface
 
	// Begin UPrimitiveComponent interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual class UBodySetup* GetBodySetup() override;
	// End UPrimitiveComponent interface.
 
	void UpdateBodySetup();
	void UpdateCollision();
protected:
 
 
 
	// Begin USceneComponent interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform & LocalToWorld) const override;
	// Begin USceneComponent interface.
 
	/** */
	TArray<FVMeshTri> GeneratedMeshTris;
 
	friend class FGeneratedMeshSceneProxy;
};