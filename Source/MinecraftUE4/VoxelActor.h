// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "VoxelActor.generated.h"

struct FMeshSection
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	int32 elementId = 0;
};

USTRUCT()
struct FBlock
{
	GENERATED_BODY()
public:
	UPROPERTY()
	int32 type;
	UPROPERTY()
	int32 health;
};

UCLASS()
class MINECRAFTUE4_API AVoxelActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialInterface *> Materials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int32 randomSeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int32 voxelSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int32 chunkLineElements = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int32 chunkXIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int32 chunkYIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	float Weight = 4.0f;

	UPROPERTY()
	int32 chunkTotalElements;

	UPROPERTY()
	int32 chunkZElements;

	UPROPERTY()
	int32 chunkLineElementsP2;

	UPROPERTY()
	int32 voxelSizeHalf;

	UPROPERTY()
	TArray<FBlock> chunkFields;

	UPROPERTY()
	UProceduralMeshComponent *proceduralComponent;

	UFUNCTION()
	TArray<int32> calculateNoise();

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	void setVoxel(FVector localPos, int32 type);

public:
	// Sets default values for this actor's properties
	AVoxelActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform &Transform) override;

	void GenerateChunk();

	void UpdateMesh();

private:
	TArray<FMeshSection> GenerateMeshSections();

	void ApplyMaterials();
};
