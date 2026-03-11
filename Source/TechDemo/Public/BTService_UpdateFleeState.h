// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_UpdateFleeState.generated.h"

/**
 * 
 */
UCLASS()
class TECHDEMO_API UBTService_UpdateFleeState : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateFleeState();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
    UPROPERTY(EditAnywhere, Category = "Flee")
    FBlackboardKeySelector TargetActorKey;

    UPROPERTY(EditAnywhere, Category = "Flee")
    FBlackboardKeySelector IsFleeingKey;

    UPROPERTY(EditAnywhere, Category = "Flee")
    float StartFleeDistance = 900.f;

    UPROPERTY(EditAnywhere, Category = "Flee")
    float StopFleeDistance = 1200.f;
};
