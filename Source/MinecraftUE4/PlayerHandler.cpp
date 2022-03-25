// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerHandler.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#define OUT

// Sets default values for this component's properties
UPlayerHandler::UPlayerHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UPlayerHandler::BeginPlay()
{
	Super::BeginPlay();

	PlaceholderBlock = GetWorld()->SpawnActor<AActor>(ActorToSpawn, FVector{}, FRotator::ZeroRotator);
	PlaceholderBlock->SetActorHiddenInGame(true);
}

// Called every frame
void UPlayerHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SetPlaceHolderBlock();
}

void UPlayerHandler::SetPlaceHolderBlock()
{
	FVector PlayerLocation;
	FRotator PlayerRotator;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerLocation, OUT PlayerRotator);

	FVector TargetPosition = PlayerLocation + UKismetMathLibrary::GetForwardVector(PlayerRotator) * Reach;

	// Get players viewpoint
	const APlayerCameraManager *PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	const FVector Start = PlayerCameraManager->GetCameraLocation();
	const FVector End = Start + PlayerCameraManager->GetActorForwardVector() * Reach;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this->GetOwner());

	bool didHit = GetWorld()->LineTraceSingleByChannel(OUT HitResult, Start, TargetPosition, ECollisionChannel::ECC_Visibility, Params, FCollisionResponseParams());

	// Draw a line from player showing reach
	// DrawDebugLine(
	// 	GetWorld(),
	// 	Start,
	// 	TargetPosition,
	// 	FColor(255, 0, 0),
	// 	false,
	// 	0.0f,
	// 	0,
	// 	5);

	AVoxelActor *HitVoxel = Cast<AVoxelActor>(HitResult.Actor);
	FVector voxelPosition = (HitResult.Location - 50.0f) + (UKismetMathLibrary::GetDirectionUnitVector(HitResult.Location, Start) * -1);

	if (didHit && HitVoxel)
	{

		PlaceholderBlock->SetActorLocation(FVector(FloorPosition(voxelPosition)) + FVector(0.0f, 0.0f, 100.0f));
		PlaceholderBlock->SetActorHiddenInGame(false);
	}
	else
	{
		PlaceholderBlock->SetActorHiddenInGame(true);
	}
}

FIntVector UPlayerHandler::FloorPosition(const FVector &position)
{
	FIntVector FlooredPosition{FMath::FloorToInt(position.X), FMath::FloorToInt(position.Y), FMath::FloorToInt(position.Z)};

	FlooredPosition.X = FMath::FloorToInt(FlooredPosition.X / 100) * 100;
	FlooredPosition.Y = FMath::FloorToInt(FlooredPosition.Y / 100) * 100;
	FlooredPosition.Z = FMath::FloorToInt(FlooredPosition.Z / 100) * 100;

	return FlooredPosition;
}
