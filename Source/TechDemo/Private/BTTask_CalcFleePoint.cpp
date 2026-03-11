// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CalcFleePoint.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_CalcFleePoint::UBTTask_CalcFleePoint()
{
	NodeName = "Calculate Flee Point";
}

EBTNodeResult::Type UBTTask_CalcFleePoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* Pawn = AICon->GetPawn();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!Pawn || !BB) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		BB->SetValueAsBool(IsFleeingKey.SelectedKeyName, false);
		return EBTNodeResult::Failed;
	}

	const FVector SelfLoc = Pawn->GetActorLocation();
	const FVector TargetLoc = TargetActor->GetActorLocation();
	FVector Dir = SelfLoc - TargetLoc;
	
	if (Dir.IsNearlyZero()) return EBTNodeResult::Failed;
	

	const bool bWasFleeing = BB->GetValueAsBool(IsFleeingKey.SelectedKeyName);
	bool bShouldFlee = bWasFleeing;
	if (bWasFleeing)
	{
		if (Dir.Size() >= StopFleeDistance)
		{
			bShouldFlee = false;
		}
	}
	else
	{
		if (Dir.Size() <= StartFleeDistance)	
		{
			bShouldFlee = true;
		}
	}

	BB->SetValueAsBool(IsFleeingKey.SelectedKeyName, bShouldFlee);

	if (!bShouldFlee)
	{
		return EBTNodeResult::Failed;
	}

	Dir.Normalize();

	FVector RawFleePoint = SelfLoc + Dir * FleeDistance;
	FNavLocation Projected;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Pawn->GetWorld());
	if (!NavSys) return EBTNodeResult::Failed;

	if (!NavSys->ProjectPointToNavigation(RawFleePoint, Projected, NavProjectExtent))
	{
		return EBTNodeResult::Failed;
	}

	BB->SetValueAsVector(BlackboardKey.SelectedKeyName, Projected.Location);

	return EBTNodeResult::Succeeded;
}
