// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AIConfigSet.generated.h"


class UBehaviorTree;
class UBlackboardData;

USTRUCT(BlueprintType)
struct FAIPerceptionConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float SightRadius = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float LoseSightRadius = 1500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	bool bEnableSight = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float PeripheralVisionAngleDegrees = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float MaxAge = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	bool bDetectEnemies = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	bool bDetectNeutrals = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	bool bDetectFriendlies = false;

};

USTRUCT(BlueprintType)
struct FAIMovementConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StoppingDistance = 100.0f;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class TECHDEMO_API UAIConfigSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
	UBehaviorTree* BehaviorTree;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
	UBlackboardData* BlackboardData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FAIPerceptionConfig PerceptionConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FAIMovementConfig MovementConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName ConfigID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	int32 Version = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FString ConfigDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Filter")
	FGameplayTagContainer AllowedTargetTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Filter")
	bool bRequireAnyAllowedTag = true;
	UFUNCTION(BlueprintCallable, Category = "Config")
	bool ValidateConfig(FString& OutError);

};
