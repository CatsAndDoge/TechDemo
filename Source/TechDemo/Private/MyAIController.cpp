// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "AIConfigSet.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "GameFramework/Character.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "GameplayTagAssetInterface.h"
#include <Kismet/GameplayStatics.h>
#include "Navigation/PathFollowingComponent.h"

AMyAIController::AMyAIController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	UAIPerceptionComponent* AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 1800.f;
	SightConfig->PeripheralVisionAngleDegrees = 70.f;
	SightConfig->SetMaxAge(2.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AMyAIController::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("MyAIController BeginPlay started"));
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("MyAIController BeginPlay"));
}

void AMyAIController::OnPossess(APawn* InPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("MyAIController OnPossess started for pawn: %s"), InPawn ? *InPawn->GetName() : TEXT("None"));
	Super::OnPossess(InPawn);
	if (!InPawn)
	{
			UE_LOG(LogTemp, Warning, TEXT("OnPossess failed: Invalid pawn"));
			return;
	}

	if (!EnsureConfigLoaded())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnPossess failed: EnsureConfigLoaded failed"));
		return;
	}

	ApplyAIConfig(CurrentConfigID);

	if (UAIPerceptionComponent* AIPerception = GetAIPerceptionComponent())
	{
		const FName FuncName = GET_FUNCTION_NAME_CHECKED(AMyAIController, HandleTargetPerceptionUpdated);
		AIPerception->OnTargetPerceptionUpdated.Remove(this, FuncName);
		FScriptDelegate Delegate;
		Delegate.BindUFunction(this, FuncName);
		AIPerception->OnTargetPerceptionUpdated.Add(Delegate);
		UE_LOG(LogTemp, Warning, TEXT("MyAIController bound HandleTargetPerceptionUpdated to OnTargetPerceptionUpdated"));
	}

	if (!bUseAIPerception)
	{
		ApplyFallbackTargetActor();
	}

	UE_LOG(LogTemp, Warning, TEXT("MyAIController possessed pawn: %s"), *InPawn->GetName());
}

void AMyAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UAIPerceptionComponent* AIPerception = GetAIPerceptionComponent())
	{
		const FName FuncName = GET_FUNCTION_NAME_CHECKED(AMyAIController, HandleTargetPerceptionUpdated);
		AIPerception->OnTargetPerceptionUpdated.Remove(this, FuncName);
	}
	Super::EndPlay(EndPlayReason);
}

void AMyAIController::ApplyAIConfig(FName ConfigID)
{	
	if (UAIConfigSet** FoundConfig = AIConfigMap.Find(ConfigID))
	{
		FString VaildateError;
		if (!(*FoundConfig)->ValidateConfig(VaildateError)) {
			UE_LOG(LogTemp, Error, TEXT("Invalid AI Config: %s"), *VaildateError);
			return;
		}

		CurrentConfigID = ConfigID;
		CurrentConfig = *FoundConfig;

		SetupSightFromConfig();
		SetupMovementFromConfig();

		if (!CurrentConfig->BlackboardData)
		{
			UE_LOG(LogTemp, Warning, TEXT("Applying AI Config failed: AI Config missing BlackboardData"));
			return;
		}

		if (!CurrentConfig->BehaviorTree)
		{
			UE_LOG(LogTemp, Warning, TEXT("Applying AI Config failed: AI Config missing BehaviorTree"));
			return;
		}

		UBlackboardComponent* OutBB = nullptr;
		const bool bUseBB = UseBlackboard(CurrentConfig->BlackboardData, OutBB);
		if (!bUseBB || !OutBB)
		{
			UE_LOG(LogTemp, Warning, TEXT("Applying AI Config failed: UseBlackboard failed"));
			return;
		}

		BlackboardComp = OutBB;

		const bool bRunBT = RunBehaviorTree(CurrentConfig->BehaviorTree);
		if (!bRunBT)
		{
			UE_LOG(LogTemp, Warning, TEXT("Applying AI Config failed: RunBehaviorTree failed"));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Applying AI Config failed: ConfigID '%s' not found in AIConfigMap"), *ConfigID.ToString());
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Applying AI Config: %s"), *ConfigID.ToString());
}

void AMyAIController::GetCurrentAIConfig(FName& OutConfigID)
{
	OutConfigID = CurrentConfigID;
	UE_LOG(LogTemp, Warning, TEXT("Current AI Config: %s"), *OutConfigID.ToString());
}

void AMyAIController::MoveToTarget(AActor* Target)
{
	if (!Target || !GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveToTarget failed: Invalid target or no pawn possessed"));
		return;
	}
	FAIMoveRequest MoveRequest(Target);
	if (CurrentConfig)
	{
		MoveRequest.SetAcceptanceRadius(CurrentConfig->MovementConfig.StoppingDistance);
	}
	MoveTo(MoveRequest);
}

void AMyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	FString ResultText;
	switch (Result.Code)
	{
	case EPathFollowingResult::Success:
		ResultText = TEXT("Success");
		break;
	case EPathFollowingResult::Blocked:
		ResultText = TEXT("Blocked");
		break;
	case EPathFollowingResult::OffPath:
		ResultText = TEXT("OffPath");
		break;
	case EPathFollowingResult::Aborted:
		ResultText = TEXT("Aborted");
		break;
	case EPathFollowingResult::Invalid:
		ResultText = TEXT("Invalid");
		break;
	default:
		ResultText = TEXT("Unknown");
		break;
	}

	const uint32 ReqId = RequestID.IsValid() ? RequestID.GetID() : 0u;
	UE_LOG(LogTemp, Warning, TEXT("MoveCompleted: RequestID=%u, Result=%s, Flags=%d"),
		ReqId, *ResultText, static_cast<int32>(Result.Flags));
}

void AMyAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!bUseAIPerception)
	{
		return;
	}
	if (!BlackboardComp || !Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Perception Warning: Invalid Blackboard or Actor"));
		return;
	}

	if (!IsValidChaseTarget(Actor))
	{
		return;
	}

	static const FName TargetActorKey(TEXT("TargetActor"));

	if (Stimulus.WasSuccessfullySensed())
	{
		BlackboardComp->SetValueAsObject(TargetActorKey, Actor);
		UE_LOG(LogTemp, Log, TEXT("Blackboard set TargetActor: %s"), *Actor->GetName());
	}
	else
	{
		UObject* CurrentTarget = BlackboardComp->GetValueAsObject(TargetActorKey);
		if (CurrentTarget == Actor)
		{
			BlackboardComp->ClearValue(TargetActorKey);
			UE_LOG(LogTemp, Log, TEXT("Blackboard clear TargetActor: %s"), *Actor->GetName());
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Perception updated for actor: %s, sensed: %s"), *Actor->GetName(), Stimulus.WasSuccessfullySensed() ? TEXT("true") : TEXT("false"));
}

void AMyAIController::SetupSightFromConfig()
{
	if (!CurrentConfig || !SightConfig || !GetAIPerceptionComponent())
	{
		return;
	}

	FAIPerceptionConfig& PConfig = CurrentConfig->PerceptionConfig;

	SightConfig->SightRadius = PConfig.SightRadius;
	SightConfig->LoseSightRadius = PConfig.LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PConfig.PeripheralVisionAngleDegrees;
	SightConfig->SetMaxAge(PConfig.MaxAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = PConfig.bDetectEnemies;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = PConfig.bDetectNeutrals;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = PConfig.bDetectFriendlies;

	UAIPerceptionComponent* AIPerception = GetAIPerceptionComponent();
	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerception->RequestStimuliListenerUpdate();
}

void AMyAIController::SetupMovementFromConfig()
{
	if (!CurrentConfig || !GetPawn())
	{
		return;
	}
	const FAIMovementConfig& MConfig = CurrentConfig->MovementConfig;
	if (ACharacter* ControllerCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* MoveComp = ControllerCharacter->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = MConfig.WalkSpeed;
		}
	}
}

bool AMyAIController::EnsureConfigLoaded()
{
	if (!CurrentConfig && !DefaultConfig.IsNull())
	{
		CurrentConfig = DefaultConfig.LoadSynchronous();
	}
	if (!CurrentConfig)
	{
		const FSoftObjectPath FallBackPath(TEXT("/Game/DemoContent/AI/Config/DA_AI_Default.DA_AI_Default"));
		CurrentConfig = Cast<UAIConfigSet>(FallBackPath.TryLoad());
	}
	if (!CurrentConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("No valid AI Config found"));
		return false;
	}

	FString VaildateError;
	if (!CurrentConfig->ValidateConfig(VaildateError))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid AI Config: %s"), *VaildateError);
		return false;
	}
	
	CurrentConfigID = CurrentConfig->ConfigID;

	return true;
}

bool AMyAIController::IsValidChaseTarget(AActor* Actor)
{
	if (!Actor || !CurrentConfig) return false;

	if (!CurrentConfig->bRequireAnyAllowedTag) return true;

	IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Actor);
	if (!TagInterface) return false;

	FGameplayTagContainer TargetTags;
	TagInterface->GetOwnedGameplayTags(TargetTags);

	bool bMatched = TargetTags.HasAny(CurrentConfig->AllowedTargetTags);
	if (!bMatched)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AI Config %s: Actor %s does not matched."), *CurrentConfig->ConfigID.ToString(), *Actor->GetName());
		return false;
	}
	
	return true;
}

void AMyAIController::ApplyFallbackTargetActor()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (IsValid(PlayerPawn))
	{
		BlackboardComp->SetValueAsObject(TEXT("TargetActor"), PlayerPawn);
	}
	else
	{
		BlackboardComp->ClearValue(TEXT("TargetActor"));
	}
}
