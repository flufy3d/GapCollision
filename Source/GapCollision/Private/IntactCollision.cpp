// Fill out your copyright notice in the Description page of Project Settings.

#include "GapCollisionPrivatePCH.h"
#include "IntactCollision.h"


#include "Landscape.h"
#include "LandscapeComponent.h"
#include "LandscapeDataAccess.h"
#include "LandscapeLayerInfoObject.h"



// Sets default values
AIntactCollision::AIntactCollision()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;

	install = true;

	done = false;

	step = 20.0;

	offset = 1.5;

	hole = 9;

	target = "Phy_green";
	
	x0 = 5;
	y0 = -5;
	x1 = -5;
	y1 = 5;
}

// Called when the game starts or when spawned
void AIntactCollision::BeginPlay()
{
	Super::BeginPlay();

	
}



// Called every frame
void AIntactCollision::Tick( float DeltaTime )
{
	FVector origin = this->GetActorLocation();
	Super::Tick( DeltaTime );
}

void AIntactCollision::PostEditMove(bool bFinished)
{
	/*
	FVector origin = this->GetActorLocation();
	UWorld* world = GetWorld();
	if (world == nullptr)
		return;

	// Find the active landscape
	TActorIterator<ALandscape> landscapeIterator(world);
	ALandscape* landscape = *landscapeIterator;


	FCollisionQueryParams collisionParams(FName(TEXT("FoliageClusterPlacementTrace")), true, this);
	collisionParams.bReturnPhysicalMaterial = true;

	FHitResult hit(ForceInit);
	if (landscape->ActorLineTraceSingle(hit, origin - FVector(0, 0, 100), origin + FVector(0, 0, 100), ECC_Visibility, collisionParams)) {
		// Do something
	}

	if(hit.PhysMaterial.IsValid())
	{
		FString a = hit.PhysMaterial->GetName();
		UE_LOG(LogTemp, Warning, TEXT("hi: %s"), *a);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("hi: null"));
	}
	*/
}
void AIntactCollision::DetectBound(FVector origin, int& _x0, int& _y0, int& _x1, int& _y1)
{
	//detect the total size;
	UWorld* world = GetWorld();
	if (world == nullptr)
		return;

	// Find the active landscape
	TActorIterator<ALandscape> landscapeIterator(world);
	ALandscape* landscape = *landscapeIterator;


	FCollisionQueryParams collisionParams(FName(TEXT("Kenshin")), true, this);
	collisionParams.bReturnPhysicalMaterial = true;
	FHitResult hit(ForceInit);


	FVector DetectOffset = FVector(0, 0, 500);
	_x0 = 1;
	_y0 = -1;
	_x1 = -1;
	_y1 = 1;


	bool found_bound;


	//find x0;
	found_bound = false;
	while (!found_bound)
	{
		FVector cursor = origin + FVector(_x0*step, 0, 0);
		if (landscape->ActorLineTraceSingle(hit, cursor + DetectOffset, cursor - DetectOffset, ECC_Visibility, collisionParams)) {
			FString layer = hit.PhysMaterial->GetName();
			if (!layer.Equals(target))
			{
				found_bound = true;
			};
		};
		_x0 += 1;
	}

	//find y0;
	found_bound = false;
	while (!found_bound)
	{
		FVector cursor = origin + FVector(0, _y0*step, 0);
		if (landscape->ActorLineTraceSingle(hit, cursor + DetectOffset, cursor - DetectOffset, ECC_Visibility, collisionParams)) {
			FString layer = hit.PhysMaterial->GetName();
			if (!layer.Equals(target))
			{
				found_bound = true;
			};
		};
		_y0 += -1;
	}

	//find x1;
	found_bound = false;
	while (!found_bound)
	{
		FVector cursor = origin + FVector(_x1*step, 0, 0);
		if (landscape->ActorLineTraceSingle(hit, cursor + DetectOffset, cursor - DetectOffset, ECC_Visibility, collisionParams)) {
			FString layer = hit.PhysMaterial->GetName();
			if (!layer.Equals(target))
			{
				found_bound = true;
			};
		};
		_x1 += -1;
	}


	//find y1;
	found_bound = false;
	while (!found_bound)
	{
		FVector cursor = origin + FVector(0, _y1*step, 0);
		if (landscape->ActorLineTraceSingle(hit, cursor + DetectOffset, cursor - DetectOffset, ECC_Visibility, collisionParams)) {
			FString layer = hit.PhysMaterial->GetName();
			if (!layer.Equals(target))
			{
				found_bound = true;
			};
		};
		_y1 += 1;
	}

}
void AIntactCollision::OnConstruction(const FTransform & Transform)
{

	FVector origin = Transform.GetLocation();
	FQuat rot = Transform.GetRotation();


	if (install)
	{
		mesh->ClearAllMeshSections();

		TArray<FVector> vertices;

		vertices.Add(FVector(x0*step, y0*step, offset));
		vertices.Add(FVector(x0*step, y1*step, offset));
		vertices.Add(FVector(x1*step, y0*step, offset));
		vertices.Add(FVector(x1*step, y1*step, offset));


		TArray<int32> Triangles;
		Triangles.Add(2);
		Triangles.Add(1);
		Triangles.Add(0);
		Triangles.Add(2);
		Triangles.Add(3);
		Triangles.Add(1);

		TArray<FVector> normals;
		normals.Add(FVector(1, 0, 0));
		normals.Add(FVector(1, 0, 0));
		normals.Add(FVector(1, 0, 0));
		normals.Add(FVector(1, 0, 0));

		TArray<FVector2D> UV0;
		UV0.Add(FVector2D(0, 0));
		UV0.Add(FVector2D(10, 0));
		UV0.Add(FVector2D(0, 10));
		UV0.Add(FVector2D(0, 0));

		TArray<FColor> vertexColors;
		vertexColors.Add(FColor(0.75, 0.75, 0.75, 1.0));
		vertexColors.Add(FColor(0.75, 0.75, 0.75, 1.0));
		vertexColors.Add(FColor(0.75, 0.75, 0.75, 1.0));
		vertexColors.Add(FColor(0.75, 0.75, 0.75, 1.0));

		TArray<FProcMeshTangent> tangents;
		tangents.Add(FProcMeshTangent(0, 1, 0));
		tangents.Add(FProcMeshTangent(0, 1, 0));
		tangents.Add(FProcMeshTangent(0, 1, 0));
		tangents.Add(FProcMeshTangent(0, 1, 0));

		mesh->CreateMeshSection(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
		mesh->CastShadow = false;
		mesh->SetHiddenInGame(true); //hide! 
	}
	else
	{

		if (done)  return;
		UWorld* world = GetWorld();
		if (world == nullptr)
			return;

		// Find the active landscape
		TActorIterator<ALandscape> landscapeIterator(world);
		ALandscape* landscape = *landscapeIterator;


		FCollisionQueryParams collisionParams(FName(TEXT("Kenshin")), true, this);
		collisionParams.bReturnPhysicalMaterial = true;
		FHitResult hit(ForceInit);
		
		FVector DetectOffset = FVector(0, 0, 10000);


		mesh->ClearAllMeshSections();


		MyPoints.Reset();
		int _ii = 0;
		for (int x = x0; x > x1; x--)
		{
			
			for (int y = y0; y < y1; y++)
			{

				FMyPoint point;
				FVector cursor = origin + FVector(x*step, y*step, 0);
				redo:
				if (landscape->ActorLineTraceSingle(hit, cursor + DetectOffset, cursor - DetectOffset, ECC_Visibility, collisionParams)) 
				{
					FString layer = hit.PhysMaterial->GetName();

					int _r = x * x + y * y;
					if (layer.Equals(target) && _r > hole)
					{
						
						point.isTarget = true;
						point.isEdge = false;
						point.index = _ii;
						_ii++;
					}
					else 
					{

						point.isTarget = false;
						point.index = -1;
					}

					
					point.Location = hit.Location - origin;

					MyPoints.Add(point);


					
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("coord: %d %d linetrace failed"), x,y);
					cursor += FVector(0.5, 0.5, 0);
					goto redo;
				};


			};

		}

		//build the mesh
		
		TArray<FVector> vertices;
		TArray<int32> Triangles;
		TArray<FVector> normals;
		TArray<FVector2D> UV0;
		TArray<FColor> vertexColors;
		TArray<FProcMeshTangent> tangents;


		int uv0 = 0;
		int uv1 = 0;
		for (int x = x0; x > x1; x--)
		{
			for (int y = y0; y < y1; y++)
			{
				FMyPoint& point = MyPoints[GetIndex(x,y)];
				if (point.index != -1)
				{
					FVector loc = point.Location;

					normals.Add(FVector(1, 0, 0));
					vertexColors.Add(FColor(0.75, 0.75, 0.75, 1.0));
					tangents.Add(FProcMeshTangent(0, 1, 0));

					int value1 = 0;
					if (uv0 % 2 == 1) value1 = 1;
					int value2 = 0;
					if (uv1 % 2 == 1) value2 = 1;
					UV0.Add(FVector2D(value1, value2));

					int type = 0;
					if (x != x0 && x > x1+1 && y != y0 && y < y1-1 )
					{
						FMyPoint& point0 = MyPoints[GetIndex(x, y - 1)];
						if (point0.isTarget)
						{
							type += 1;
						}
						else
						{
							point.isEdge = true;
						}
						FMyPoint& point1 = MyPoints[GetIndex(x - 1, y - 1)];
						if (point1.isTarget)
						{
							type += 3;
						}
						else
						{
							point.isEdge = true;
						}
						FMyPoint& point2 = MyPoints[GetIndex(x - 1, y )];
						if (point2.isTarget)
						{
							type += 7;
						}
						else
						{
							point.isEdge = true;
						}
						if (point.isTarget)
						{
							type += 15;
						}

						if (type == 26)
						{
							Triangles.Add(point0.index);
							Triangles.Add(point1.index);
							Triangles.Add(point.index);

							Triangles.Add(point1.index);
							Triangles.Add(point2.index);
							Triangles.Add(point.index);

						}
						else if (type == 25)
						{
							Triangles.Add(point1.index);
							Triangles.Add(point2.index);
							Triangles.Add(point.index);
						}
						else if (type == 11)
						{
							Triangles.Add(point0.index);
							Triangles.Add(point1.index);
							Triangles.Add(point2.index);
						}
						else if (type == 23)
						{
							Triangles.Add(point0.index);
							Triangles.Add(point2.index);
							Triangles.Add(point.index);
						}
						else if (type == 19)
						{
							Triangles.Add(point0.index);
							Triangles.Add(point1.index);
							Triangles.Add(point.index);
						}

						FMyPoint& point3 = MyPoints[GetIndex(x + 1, y - 1)];
						if (!point3.isTarget) point.isEdge = true;
						FMyPoint& point4 = MyPoints[GetIndex(x + 1, y )];
						if (!point4.isTarget) point.isEdge = true;
						FMyPoint& point5 = MyPoints[GetIndex(x + 1, y + 1)];
						if (!point5.isTarget) point.isEdge = true;
						FMyPoint& point6 = MyPoints[GetIndex(x , y + 1)];
						if (!point6.isTarget) point.isEdge = true;
						FMyPoint& point7 = MyPoints[GetIndex(x - 1, y + 1)];
						if (!point7.isTarget) point.isEdge = true;
						

					}
					else
					{
						point.isEdge = true;
					}

					if (!point.isEdge)
					{
						loc.Z += offset;
					}
					vertices.Add(loc);
					
				}

				uv1++;
			}

			uv0++;
		}

		mesh->CreateMeshSection(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
		mesh->CastShadow = false;
		mesh->SetHiddenInGame(true); //hide! 


	}

	

}