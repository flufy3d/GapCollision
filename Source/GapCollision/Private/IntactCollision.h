// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProceduralMeshComponent.h"
#include "IntactCollision.generated.h"


USTRUCT()
struct FMyPoint
{
	GENERATED_USTRUCT_BODY()

	FVector Location;
	bool isTarget;
	bool isEdge;
	int index;

	FMyPoint()
	{
	}

	FMyPoint(const FVector& location, const bool& _isTarget, const bool& _isEdge, int _index)
		: Location(location)
		, isTarget(_isTarget)
		, isEdge(_isEdge)
		, index(_index)
	{}
};

UCLASS()
class AIntactCollision : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIntactCollision();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostEditMove(bool bFinished) override;

	UPROPERTY(VisibleAnywhere, Category = Materials)
		UProceduralMeshComponent * mesh;

	UPROPERTY(EditAnywhere, Category = Kenshin)
		bool install;

	UPROPERTY(EditAnywhere, Category = Kenshin)
		float step;

	UPROPERTY(EditAnywhere, Category = Kenshin)
		float offset;

	UPROPERTY(EditAnywhere, Category = Kenshin)
		int hole;

	UPROPERTY(EditAnywhere, Category = Kenshin)
		FString target;

	UPROPERTY(EditAnywhere, Category = Kenshin)
		int x0;

	UPROPERTY(EditAnywhere, Category = Kenshin)
		int y0;

	UPROPERTY(EditAnywhere, Category = Kenshin)
		int x1;

	UPROPERTY(EditAnywhere, Category = Kenshin)
		int y1;

	UPROPERTY(EditAnywhere, Category = Kenshin)
		bool done;

private:
	void DetectBound(FVector origin, int& x0, int& y0, int& x1, int& y1);

	inline int GetIndex(int x, int y)
	{
		int ret = 0;
		int local_x = -(x - x0);
		int local_y = (y - y0);
		ret = local_x * (y1 - y0) + local_y;
		return ret;
	}

	TArray<FMyPoint> MyPoints;

	
};
