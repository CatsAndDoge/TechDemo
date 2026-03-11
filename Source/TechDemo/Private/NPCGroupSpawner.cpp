// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCGroupSpawner.h"
#include "Components/BoxComponent.h"
#include "NPCGroupConfigAsset.h"
#include "GameFramework/Character.h"
#include "MyAIController.h"
#include "AIConfigSet.h"
#include "NavigationSystem.h"

// Sets default values
ANPCGroupSpawner::ANPCGroupSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetupAttachment(Root);
	SpawnArea->SetBoxExtent(FVector(500.f, 500.f, 100.f));
	SpawnArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ANPCGroupSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoSpawnOnBeginPlay)
	{
		SpawnAllGroups();
	}
	
}

void ANPCGroupSpawner::SpawnAllGroups()
{
	if (!NPCGroupConfig)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPCGroupSpawner: NPCGroupConfig is null"));
		return;
	}

	GroupControllers.Empty();
	GroupDefaultConfigs.Empty();
	GroupCurrentConfigs.Empty();

	TArray<FVector> SpawnedPoints;
	for (const FNPCGroupSpawnRule& Rule: NPCGroupConfig->Groups)
	{
		int32 SuccessedCount = 0;
		for (int32 i = 0; i < Rule.SpawnCount; i++)
		{
			if (TrySpawnOne(Rule.GroupID, Rule.CharacterClass, Rule.DefaultAIConfig, Rule.MaxSpawnRetryCount, Rule.MinDistanceBetweenNPC, SpawnedPoints))
			{
				SuccessedCount++;
			}
			UE_LOG(LogTemp, Warning, TEXT("NPCGroupSpawner: Group:%s, Spawned %d/%d"), *Rule.GroupID.ToString(), SuccessedCount, Rule.SpawnCount);
		}
	}
}

bool ANPCGroupSpawner::SwitchGroupLogic(FName GroupID, UAIConfigSet* TargetConfig)
{
	if (GroupID.IsNone() || !TargetConfig)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPCGroupSpawner: Invalid parameters for SwitchGroupLogic"));
		return false;
	}

	TArray<TWeakObjectPtr<AMyAIController>>* Controllers = GroupControllers.Find(GroupID);
	if (!Controllers)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPCGroupSpawner: GroupID:%s not found for SwitchGroupLogic"), *GroupID.ToString());
		return false;
	}

	ClearupInvalidControllers(GroupID);

	int32 Applied = 0;
	for (TWeakObjectPtr<AMyAIController>& WeakCon : *Controllers)
	{
		if (AMyAIController* AICon = WeakCon.Get())
		{
			AICon->ApplyAIConfig(TargetConfig->ConfigID);
			Applied++;
		}
	}

	GroupCurrentConfigs.FindOrAdd(GroupID) = TargetConfig;
	UE_LOG(LogTemp, Warning, TEXT("NPCGroupSpawner: Group:%s, Applied new logic to %d controllers"), *GroupID.ToString(), Applied);

	return Applied > 0;
}

bool ANPCGroupSpawner::SwitchGroupLogicByConfigID(FName GroupID, FName ConfigID)
{
	if (!NPCGroupConfig || GroupID.IsNone() || ConfigID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("NPCGroupSpawner: Invalid parameters for SwitchGroupLogicByConfig"))
		return false;
	}

	const FNPCGroupSpawnRule* Rule = NPCGroupConfig->Groups.FindByPredicate(
		[GroupID](const FNPCGroupSpawnRule& R) { return R.GroupID == GroupID; });
	if (!Rule)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPCGroupSpawner: Config ID not Found"));
		return false;
	}

	if (const UAIConfigSet* const* Found = Rule->OtherConfigs.Find(ConfigID))
	{
		return SwitchGroupLogic(GroupID, const_cast<UAIConfigSet*>(*Found));
	}
	return false;
}

bool ANPCGroupSpawner::ExitCurrentGroupLogic(FName GroupID)
{
	bool bRevertResult = RevertDefaultGroupLogic(GroupID);
	return bRevertResult;
}

bool ANPCGroupSpawner::RevertDefaultGroupLogic(FName GroupID)
{
	TObjectPtr<UAIConfigSet>*  DefaultConfig = GroupDefaultConfigs.Find(GroupID);
	if (!DefaultConfig || !(*DefaultConfig))
	{
		UE_LOG(LogTemp, Warning, TEXT("NPCGroupSpawner: Default config not found for GroupID:%s"), *GroupID.ToString());
		return false;
	}
	return SwitchGroupLogic(GroupID, *DefaultConfig);
}

bool ANPCGroupSpawner::TrySpawnOne(FName GroupID, TSubclassOf<ACharacter> CharacterClass, UAIConfigSet* AIConfig, int32 MaxRetry, float MinDistance, TArray<FVector>& SpawnedPoints)
{
	if (!CharacterClass || !AIConfig)
	{
		return false;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}
	
	for (int32 Retry = 0; Retry < MaxRetry; Retry++)
	{
		FVector SpawnLocation;
		if (!FindRandomPointInArea(SpawnLocation))
		{
			continue;
		}
		if (!IsFarEnoughFromOthers(SpawnLocation, SpawnedPoints, MinDistance))
		{
			continue;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		ACharacter* Spawned = World->SpawnActor<ACharacter>(CharacterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		if (!Spawned)
		{
			continue;
		}
		Spawned->SpawnDefaultController();

		if (AMyAIController* AIController = Cast<AMyAIController>(Spawned->GetController()))
		{
			AIController->ApplyAIConfig(AIConfig->ConfigID);
			RegisterControllerToGroup(GroupID, AIController, AIConfig);
			UE_LOG(LogTemp, Warning, TEXT("NPCGroupSpawner: Rigister Group:%s "), *GroupID.ToString());
		}

		SpawnedPoints.Add(SpawnLocation);

		return true;
	}

	return false;
}

bool ANPCGroupSpawner::FindRandomPointInArea(FVector& OutPoint) const
{
	if (!SpawnArea) return false;
	
	const FVector Origin = SpawnArea->Bounds.Origin;
	const FVector Extent = SpawnArea->Bounds.BoxExtent;
	FVector Candidate = Origin + FVector(
		FMath::FRandRange(-Extent.X, Extent.X),
		FMath::FRandRange(-Extent.Y, Extent.Y),
		0
	);

	if (!bProjectToNavmesh)
	{
		OutPoint = Candidate;
		return true;
	}

	UNavigationSystemV1* Navsys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!Navsys)
	{
		return false;
	}

	FNavLocation Projected;
	const bool bProject = Navsys->ProjectPointToNavigation(Candidate, Projected, FVector(200.f, 200.f, 500.f));
	if (!bProject) return false;

	OutPoint = Projected.Location;

	return true;
}

bool ANPCGroupSpawner::IsFarEnoughFromOthers(const FVector& Candidate, const TArray<FVector>& SpawnedPoints, float MinDistance) const
{
	if (MinDistance <= 0.f) return true;
	for (const FVector& point : SpawnedPoints)
	{
		if (FVector::DistSquared2D(point, Candidate) < FMath::Square(MinDistance))
		{
			return false;
		}
	}
	return true;
}

void ANPCGroupSpawner::RegisterControllerToGroup(FName GroupID, AMyAIController* AICon, UAIConfigSet* DefaultConfig)
{
	if (!AICon || !DefaultConfig) return;
	GroupControllers.FindOrAdd(GroupID).Add(AICon);
	GroupDefaultConfigs.FindOrAdd(GroupID) = DefaultConfig;
	GroupCurrentConfigs.FindOrAdd(GroupID) = DefaultConfig;
}

void ANPCGroupSpawner::ClearupInvalidControllers(FName GroupID)
{
	if (TArray<TWeakObjectPtr<AMyAIController>>* Controllers = GroupControllers.Find(GroupID))
	{
		Controllers->RemoveAll([](const TWeakObjectPtr<AMyAIController>& WeakCon)
		{
			return !WeakCon.IsValid();
		});
	}
}

// Called every frame
void ANPCGroupSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

