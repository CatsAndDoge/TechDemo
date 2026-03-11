// Copyright Epic Games, Inc. All Rights Reserved.

#include "TechDemoGameMode.h"
#include "TechDemoPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ATechDemoGameMode::ATechDemoGameMode()
{
	UE_LOG(LogTemp, Log, TEXT("TechDemoGameMode constructor started"));
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		UE_LOG(LogTemp, Log, TEXT("Successfully found player pawn blueprint class: %s"), *PlayerPawnBPClass.Class->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find player pawn blueprint class. Please ensure the blueprint exists and the path is correct."));
	}
	PlayerControllerClass = ATechDemoPlayerController::StaticClass();
}