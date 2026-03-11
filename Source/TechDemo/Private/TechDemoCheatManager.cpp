// Fill out your copyright notice in the Description page of Project Settings.


#include "TechDemoCheatManager.h"
#include "MyAIController.h"
#include "EngineUtils.h"
#include <NPCGroupSpawner.h>

void UTechDemoCheatManager::AISetConfig(const FString& ConfigID)
{
	if (ConfigID.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Usage: AISetConfig <ConfigId>"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("AISetConfig failed: World is null"));
		return;
	}

	int32 AppliedCount = 0;
	const FName ConfigFName(*ConfigID);

	for (TActorIterator<AMyAIController> It(World); It; ++It)
	{
		AMyAIController* AIController = *It;
		if (AIController)
		{
			AIController->ApplyAIConfig(ConfigFName);
			AppliedCount++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("AISetConfig applied to %d AI Controllers"), AppliedCount);
}

void UTechDemoCheatManager::NPCSwitchGroupLogic(const FString& GroupID, const FString& ConfigID)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPCSwitchGroupLogic failed: World is null"));
		return;
	}

	ANPCGroupSpawner* Spawner = nullptr;
	for (TActorIterator<ANPCGroupSpawner> It(World); It; ++It)
	{
		Spawner = *It;
		break;
	}

	if (!Spawner)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPCSwitchGroupLogic failed: No NPCGroupSpawner found"));
		return;
	}

	Spawner->SwitchGroupLogicByConfigID(FName(GroupID), FName(ConfigID));
}

void UTechDemoCheatManager::NPCRevertGroupLogic(const FString& GroupID)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPCRevertGroupLogic failed: World is null"));
		return;
	}

	ANPCGroupSpawner* Spawner = nullptr;
	for (TActorIterator<ANPCGroupSpawner> It(World); It; ++It)
	{
		Spawner = *It;
		break;
	}

	if (!Spawner)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPCRevertGroupLogic failed: No NPCGroupSpawner found"));
		return;
	}

	Spawner->RevertDefaultGroupLogic(FName(GroupID));
}
