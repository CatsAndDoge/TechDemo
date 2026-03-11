// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "TechDemoCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class TECHDEMO_API UTechDemoCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
public:
		UFUNCTION(Exec, Category = "TechDemoCheats")
		void AISetConfig(const FString& ConfigID);
		UFUNCTION(Exec, Category = "TechDemoCheats")
		void NPCSwitchGroupLogic(const FString& GroupID, const FString& ConfigID);
		UFUNCTION(Exec, Category = "TechDemoCheats")
		void NPCRevertGroupLogic(const FString& GroupID);
};
