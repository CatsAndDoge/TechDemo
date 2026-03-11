// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIConfigSet.h"
#include "PatrolAIConfigSet.generated.h"

/**
 * 
 */
UCLASS()
class TECHDEMO_API UPatrolAIConfigSet : public UAIConfigSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	FName PatrolRouteTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	bool bLoopPatrol = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	bool bRandomStart = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float WaitTime = 0.f;
};
