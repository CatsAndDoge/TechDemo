// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateFleeState.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateFleeState::UBTService_UpdateFleeState()
{
    NodeName = TEXT("Update Flee State");
    Interval = 0.2f;
    RandomDeviation = 0.0f;
}

void UBTService_UpdateFleeState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AICon = OwnerComp.GetAIOwner();
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!AICon || !BB) return;

    APawn* Pawn = AICon->GetPawn();
    AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!Pawn || !Target)
    {
        BB->SetValueAsBool(IsFleeingKey.SelectedKeyName, false);
        return;
    }

    const float Dist = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
    const bool bWasFleeing = BB->GetValueAsBool(IsFleeingKey.SelectedKeyName);

    bool bNowFleeing = bWasFleeing;
    if (bWasFleeing)
    {
        if (Dist >= StopFleeDistance)
        {
            bNowFleeing = false;
        }
    }
    else
    {
        if (Dist <= StartFleeDistance)
        {
            bNowFleeing = true;
        }
    }

    BB->SetValueAsBool(IsFleeingKey.SelectedKeyName, bNowFleeing);
}