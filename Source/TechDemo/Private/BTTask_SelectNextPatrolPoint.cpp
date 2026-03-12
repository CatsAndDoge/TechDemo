// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SelectNextPatrolPoint.h"
#include "AIController.h"
#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "PatrolAIConfigSet.h"
#include <Kismet/GameplayStatics.h>

UBTTask_SelectNextPatrolPoint::UBTTask_SelectNextPatrolPoint()
{
	NodeName = "Select Next Patrol Point";
}

EBTNodeResult::Type UBTTask_SelectNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AICon = OwnerComp.GetAIOwner();
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!AICon || !BB)
    {
        return EBTNodeResult::Failed;
    }

    AMyAIController* MyCon = Cast<AMyAIController>(AICon);
    if (!MyCon || !MyCon->CurrentConfig)
    {
        return EBTNodeResult::Failed;
    }
    
	UPatrolAIConfigSet* PatrolConfig = Cast<UPatrolAIConfigSet>(MyCon->CurrentConfig);
    if (!PatrolConfig || PatrolConfig->PatrolRouteTag.IsNone())
    {
        return EBTNodeResult::Failed;
	}

    TArray<AActor*> PatrolPoints;
    UGameplayStatics::GetAllActorsWithTag(AICon, PatrolConfig->PatrolRouteTag, PatrolPoints);

    if (PatrolPoints.Num() == 0)
    {
        return EBTNodeResult::Failed;
    }

    PatrolPoints.Sort([](const AActor& A, const AActor& B)
    {
        return A.GetName() < B.GetName();
    });

	int32 CurrentIndex = BB->GetValueAsInt(PatrolIndexKey.SelectedKeyName);
    if (CurrentIndex <= 0 || CurrentIndex >= PatrolPoints.Num())
    {
        CurrentIndex = PatrolConfig->bRandomStart ? FMath::RandRange(0, PatrolPoints.Num() - 1) : 0;
    }

    AActor* TargetPoint = PatrolPoints[CurrentIndex];
    if (!TargetPoint)
    {
        return EBTNodeResult::Failed;
    }

    BB->SetValueAsVector(BlackboardKey.SelectedKeyName, TargetPoint->GetActorLocation());

    int32 NextIndex = CurrentIndex + 1;
    if (NextIndex >= PatrolPoints.Num())
    {
        NextIndex = PatrolConfig->bLoopPatrol ? 0 : CurrentIndex;
    }

    BB->SetValueAsInt(PatrolIndexKey.SelectedKeyName, NextIndex);

    return EBTNodeResult::Succeeded;
}
