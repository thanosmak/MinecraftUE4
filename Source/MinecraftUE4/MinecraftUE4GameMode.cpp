// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinecraftUE4GameMode.h"
#include "MinecraftUE4HUD.h"
#include "MinecraftUE4Character.h"
#include "UObject/ConstructorHelpers.h"

AMinecraftUE4GameMode::AMinecraftUE4GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMinecraftUE4HUD::StaticClass();
}
