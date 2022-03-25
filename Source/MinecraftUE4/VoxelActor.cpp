// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelActor.h"
#include "SimplexNoiseBPLibrary.h"

TArray<FBlock> chunkFields;
const int32 bTriangles[] = {2, 1, 0, 0, 3, 2};
const FVector2D bUVs[] = {FVector2D(0.000000, 0.000000), FVector2D(0.000000, 1.000000), FVector2D(1.000000, 1.000000), FVector2D(1.000000, 0.000000)};
const FVector bNormals0[] = {FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1)};
const FVector bNormals1[] = {FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1)};
const FVector bNormals2[] = {FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0)};
const FVector bNormals3[] = {FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0)};
const FVector bNormals4[] = {FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0)};
const FVector bNormals5[] = {FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0)};
const FVector bMask[] = {FVector(0.000000, 0.000000, 1.000000), FVector(0.000000, 0.000000, -1.000000), FVector(0.000000, 1.000000, 0.000000), FVector(0.000000, -1.000000, 0.000000), FVector(1.000000, 0.000000, 0.000000), FVector(-1.000000, 0.000000, 0.000000)};

// Sets default values
AVoxelActor::AVoxelActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVoxelActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVoxelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVoxelActor::OnConstruction(const FTransform &Transform)
{
	//// Random Seed Generation
	//// DOESN'T WORK WELL

	// randomSeed = FMath::RandRange(0, 12000);
	// FMath::RandInit(randomSeed);
	// USimplexNoiseBPLibrary::setNoiseSeed(randomSeed);

	chunkZElements = 80;
	chunkLineElementsP2 = chunkLineElements * chunkLineElements;
	chunkTotalElements = chunkLineElementsP2 * chunkZElements;
	voxelSizeHalf = voxelSize / 2;

	FString string = "Voxel_" + FString::FromInt(chunkXIndex) + "_" + FString::FromInt(chunkYIndex);
	FName name = FName(*string);
	proceduralComponent = NewObject<UProceduralMeshComponent>(this, name);
	proceduralComponent->RegisterComponent();

	RootComponent = proceduralComponent;
	RootComponent->SetWorldTransform(Transform);

	Super::OnConstruction(Transform);

	GenerateChunk();
	UpdateMesh();
}

void AVoxelActor::GenerateChunk()
{
	chunkFields.SetNumUninitialized(chunkTotalElements);
	TArray<int32> noise = calculateNoise();

	for (int32 x = 0; x < chunkLineElements; x++)
	{
		for (int32 y = 0; y < chunkLineElements; y++)
		{
			for (int32 z = 0; z < chunkZElements; z++)
			{
				int32 index = x + (y * chunkLineElements) + (z * chunkLineElementsP2);

				if (z == 30 + noise[x + y * chunkLineElements])
					if (noise[x + y * chunkLineElements] > 6)
					{
						// Snow block
						chunkFields[index].type = 2;
						chunkFields[index].health = 1;
					}
					else
					{
						// Grass block
						chunkFields[index].type = 1;
						chunkFields[index].health = 2;
					}
				else if (z == 29 + noise[x + y * chunkLineElements])
				{
					// Dirt block
					chunkFields[index].type = 3;
					chunkFields[index].health = 3;
				}
				else if (z < 29 + noise[x + y * chunkLineElements])
				{
					// Rock block
					chunkFields[index].type = 4;
					chunkFields[index].health = 4;
				}
				else
				{
					// "Invisible" block
					chunkFields[index].type = 0;
					chunkFields[index].health = 0;
				}
			}
		}
	}

	ApplyMaterials();
}

void AVoxelActor::UpdateMesh()
{
	TArray<FMeshSection> meshSections = GenerateMeshSections();

	proceduralComponent->ClearAllMeshSections();
	for (int i = 0; i < meshSections.Num(); i++)
	{
		if (meshSections[i].Vertices.Num() > 0)
			proceduralComponent->CreateMeshSection(i, meshSections[i].Vertices, meshSections[i].Triangles, meshSections[i].Normals, meshSections[i].UVs, meshSections[i].VertexColors, meshSections[i].Tangents, true);
	}
}

TArray<FMeshSection> AVoxelActor::GenerateMeshSections()
{
	TArray<FMeshSection> meshSections;
	meshSections.SetNum(Materials.Num());
	int32 el_num = 0;

	for (int32 x = 0; x < chunkLineElements; x++)
	{
		for (int32 y = 0; y < chunkLineElements; y++)
		{
			for (int32 z = 0; z < chunkZElements; z++)
			{
				int32 index = x + (y * chunkLineElements) + (z * chunkLineElementsP2);
				int32 voxelType = chunkFields[index].type;

				if (voxelType > 0)
				{
					voxelType--;

					TArray<FVector> &Vertices = meshSections[voxelType].Vertices;
					TArray<int32> &Triangles = meshSections[voxelType].Triangles;
					TArray<FVector> &Normals = meshSections[voxelType].Normals;
					TArray<FVector2D> &UVs = meshSections[voxelType].UVs;
					TArray<FColor> &VertexColors = meshSections[voxelType].VertexColors;
					TArray<FProcMeshTangent> &Tangents = meshSections[voxelType].Tangents;
					int32 elementId = meshSections[voxelType].elementId;

					// add faces, vertices, UVs and Normals
					int triangle_num = 0;
					for (int i = 0; i < 6; i++)
					{
						int newIndex = index + bMask[i].X + (bMask[i].Y * chunkLineElements) + (bMask[i].Z * chunkLineElementsP2);
						bool flag = false;

						if (voxelType >= 20)
							flag = true;
						else if (
							(x + bMask[i].X < chunkLineElements) &&
							(x + bMask[i].X >= 0) &&
							(y + bMask[i].Y < chunkLineElements) &&
							(y + bMask[i].Y >= 0))
						{
							if (newIndex < chunkFields.Num() && newIndex >= 0)
								if (chunkFields[newIndex].type < 1)
									flag = true;
						}
						else
							flag = true;

						if (flag)
						{
							Triangles.Add(bTriangles[0] + triangle_num + elementId);
							Triangles.Add(bTriangles[1] + triangle_num + elementId);
							Triangles.Add(bTriangles[2] + triangle_num + elementId);
							Triangles.Add(bTriangles[3] + triangle_num + elementId);
							Triangles.Add(bTriangles[4] + triangle_num + elementId);
							Triangles.Add(bTriangles[5] + triangle_num + elementId);
							triangle_num += 4;

							switch (i)
							{
							case 0:
							{
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals0, ARRAY_COUNT(bNormals0));
								break;
							}
							case 1:
							{
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals1, ARRAY_COUNT(bNormals1));
								break;
							}
							case 2:
							{
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals2, ARRAY_COUNT(bNormals2));
								break;
							}
							case 3:
							{
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals3, ARRAY_COUNT(bNormals3));
								break;
							}
							case 4:
							{
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals4, ARRAY_COUNT(bNormals4));
								break;
							}
							case 5:
							{
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals5, ARRAY_COUNT(bNormals5));
								break;
							}
							}

							UVs.Append(bUVs, ARRAY_COUNT(bUVs));
							FColor color = FColor(255, 255, 255, i);
							VertexColors.Add(color);
							VertexColors.Add(color);
							VertexColors.Add(color);
							VertexColors.Add(color);
						}
					}
					el_num += triangle_num;
					meshSections[voxelType].elementId += triangle_num;
				}
			}
		}
	}

	return meshSections;
}

void AVoxelActor::ApplyMaterials()
{
	int i = 0;
	while (i < Materials.Num())
	{
		float z = proceduralComponent->GetOwner()->GetActorLocation().Z;
		proceduralComponent->SetMaterial(i, Materials[i]);
		i++;
	}
}

TArray<int32> AVoxelActor::calculateNoise()
{
	TArray<int32> NoiseResult;
	NoiseResult.SetNum(chunkLineElementsP2);
	float _X = 0.0f;
	float _Y = 0.0f;
	float Result = 0.0f;

	for (int32 Y = 0; Y < chunkLineElements; ++Y)
	{
		for (int32 X = 0; X < chunkLineElements; ++X)
		{
			// 1
			_X = (chunkXIndex * chunkLineElements + X) * 0.01f;
			_Y = (chunkYIndex * chunkLineElements + Y) * 0.01f;
			Result += USimplexNoiseBPLibrary::SimplexNoise2D(_X, _Y) * Weight;

			// 2
			_X = (chunkXIndex * chunkLineElements + X) * 0.01f;
			_Y = (chunkYIndex * chunkLineElements + Y) * 0.01f;
			Result += USimplexNoiseBPLibrary::SimplexNoise2D(_X, _Y) * Weight * 2.0f;

			// 3
			_X = (chunkXIndex * chunkLineElements + X) * 0.004f;
			_Y = (chunkYIndex * chunkLineElements + Y) * 0.004f;
			Result += USimplexNoiseBPLibrary::SimplexNoise2D(_X, _Y) * Weight * 4.0f;

			// 4
			_X = (chunkXIndex * chunkLineElements + X) * 0.05f;
			_Y = (chunkYIndex * chunkLineElements + Y) * 0.05f;
			Result += FMath::Clamp(USimplexNoiseBPLibrary::SimplexNoise2D(_X, _Y) * Weight, 0.0f, 5.0f);

			// Finalize
			NoiseResult[Y * chunkLineElements + X] = FMath::Floor(Result);
			Result = 0.0f;
		}
	}

	return NoiseResult;
}

void AVoxelActor::setVoxel(FVector localPos, int32 type)
{
	int32 x = localPos.X / voxelSize;
	int32 y = localPos.Y / voxelSize;
	int32 z = localPos.Z / voxelSize;
	int32 index = x + (y * chunkLineElements) + (z * chunkLineElementsP2);

	// If we set the voxel to be removed
	if (type == 0)
	{
		chunkFields[index].health--;

		// Remove voxel if health is 0
		if (chunkFields[index].health == 0)
		{
			chunkFields[index].type = type;
			UpdateMesh();
		}
	}
	else
	{
		chunkFields[index].type = type;

		// Set health to new voxel
		switch (type)
		{
		case 1:
			chunkFields[index].health = 2;
			break;
		case 2:
			chunkFields[index].health = 1;
			break;
		case 3:
			chunkFields[index].health = 3;
			break;
		case 4:
			chunkFields[index].health = 4;
			break;

		default:
			break;
		}

		UpdateMesh();
	}
}
