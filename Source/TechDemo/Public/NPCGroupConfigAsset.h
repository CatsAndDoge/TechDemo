// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIConfigSet.h"
#include "GameFramework/Character.h"
#include "NPCGroupConfigAsset.generated.h"

USTRUCT(BlueprintType)
struct FNPCGroupSpawnRule
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Group")
	FName GroupID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Group")
	TSubclassOf<ACharacter> CharacterClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Group")
	UAIConfigSet* DefaultAIConfig = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Group")
	TMap<FName, UAIConfigSet*> OtherConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Group", meta = (ClampMin = "1"))
	int32 SpawnCount = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Group", meta = (ClampMin = "0"))
	float MinDistanceBetweenNPC = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Group", meta = (ClampMin = "1"))
	int32 MaxSpawnRetryCount = 5;
};

/**
 * 
 */
UCLASS(BlueprintType)
class TECHDEMO_API UNPCGroupConfigAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Group")
	TArray<FNPCGroupSpawnRule> Groups;

	UFUNCTION(BlueprintCallable, Category = "Validation")
	bool ValidateConfig(FString& OutError) const;
};
