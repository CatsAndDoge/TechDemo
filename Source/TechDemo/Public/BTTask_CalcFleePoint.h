// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_CalcFleePoint.generated.h"

/**
 * 
 */
UCLASS()
class TECHDEMO_API UBTTask_CalcFleePoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_CalcFleePoint();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	UPROPERTY(EditAnywhere, Category = "Flee")
	FBlackboardKeySelector TargetActorKey;
    UPROPERTY(EditAnywhere, Category = "Flee")
    FBlackboardKeySelector IsFleeingKey;
	UPROPERTY(EditAnywhere, Category = "Flee")
    float FleeDistance = 200.f;
    UPROPERTY(EditAnywhere, Category = "Flee")
    float StartFleeDistance = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Flee")
    float StopFleeDistance = 200.0f;

    UPROPERTY(EditAnywhere, Category = "Flee")
    FVector NavProjectExtent = FVector(200.0f, 200.0f, 500.0f);
};
