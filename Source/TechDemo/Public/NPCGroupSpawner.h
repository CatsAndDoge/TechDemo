// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NPCGroupSpawner.generated.h"

class UBoxComponent;
class UNPCGroupConfigAsset;
class UAIConfigSet;
class AMyAIController;

UCLASS()
class TECHDEMO_API ANPCGroupSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPCGroupSpawner();
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SpawnAllGroups();
	UFUNCTION(BlueprintCallable, Category = "Group Logic")
	bool SwitchGroupLogic(FName GroupID, UAIConfigSet* TargetConfig);
	UFUNCTION(BlueprintCallable, Category = "Group Logic")
	bool SwitchGroupLogicByConfigID(FName GroupID, FName ConfigID);
	UFUNCTION(BlueprintCallable, Category = "Group Logic")
	bool ExitCurrentGroupLogic(FName GroupID);
	UFUNCTION(BlueprintCallable, Category = "Group Logic")
	bool RevertDefaultGroupLogic(FName GroupID);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	UBoxComponent* SpawnArea;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	UNPCGroupConfigAsset* NPCGroupConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	bool bAutoSpawnOnBeginPlay = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	bool bProjectToNavmesh = true;

	bool TrySpawnOne(
		FName GroupID, TSubclassOf<ACharacter> CharacterClass ,UAIConfigSet* AIConfig,
		int32 MaxRetry, float MinDistance, TArray<FVector>& SpawnedPoints
	);
	bool FindRandomPointInArea(FVector& OutPoint) const;
	bool IsFarEnoughFromOthers(const FVector& Candidate, const TArray<FVector>& SpawnedPoints, float MinDistance) const;

	void RegisterControllerToGroup(FName GroupID, AMyAIController* AICon, UAIConfigSet* DefaultConfig);
	void ClearupInvalidControllers(FName GroupID);

private:
	TMap<FName, TArray<TWeakObjectPtr<AMyAIController>>> GroupControllers;
	TMap<FName, TObjectPtr<UAIConfigSet>> GroupDefaultConfigs;
	TMap<FName, TObjectPtr<UAIConfigSet>> GroupCurrentConfigs;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
