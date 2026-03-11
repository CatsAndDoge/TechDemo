// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Delegates/Delegate.h"
#include "MyAIController.generated.h"


class UAISenseConfig_Sight;

/**
 * 
 */
UCLASS()
class TECHDEMO_API AMyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMyAIController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "AI Config")
	void ApplyAIConfig(FName ConfigID);
	UFUNCTION(BlueprintCallable, Category = "AI Config")
	void GetCurrentAIConfig(FName& ConfigID);
	UFUNCTION(BlueprintCallable, Category = "AI Movement")
	void MoveToTarget(AActor* Target);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
	FName CurrentConfigID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
	class UAIConfigSet* CurrentConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Config")
	TSoftObjectPtr<UAIConfigSet> DefaultConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Config")
	TMap<FName, UAIConfigSet*> AIConfigMap;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "AI")
	class UBlackboardComponent* BlackboardComp;
	class UPathFollowingComponent* PathFollowingComp;
	UPROPERTY()
	class UAISenseConfig_Sight* SightConfig;
	FDelegateHandle PerceptionUpdateHandle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Config")
	bool bUseAIPerception = true;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	void SetupSightFromConfig();
	void SetupMovementFromConfig();
	bool EnsureConfigLoaded();
	bool IsValidChaseTarget(AActor* Actor);
	void ApplyFallbackTargetActor();
};
