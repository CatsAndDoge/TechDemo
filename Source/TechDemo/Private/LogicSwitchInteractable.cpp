// Fill out your copyright notice in the Description page of Project Settings.


#include "LogicSwitchInteractable.h"
#include "Components/BoxComponent.h"
#include <Kismet/GameplayStatics.h>
#include "GameplayTagAssetInterface.h"
#include "NPCGroupSpawner.h"
#include "Components/TextRenderComponent.h"

// Sets default values
ALogicSwitchInteractable::ALogicSwitchInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);

	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	BoxTrigger->SetupAttachment(MeshComponent);
	BoxTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PromptText"));
	TextComponent->SetupAttachment(MeshComponent);
	TextComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 140.0f));
	TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextComponent->SetWorldSize(36.0f);
	TextComponent->SetTextRenderColor(FColor::White);
	TextComponent->SetHiddenInGame(true);
}

bool ALogicSwitchInteractable::Interact(AActor* Interactor)
{
	if (!Interactor || !NPCGroupSpawner || !CachedOverlappingPlayerPawn.IsValid())
	{
		return false;
	}

	if (SwitchLogicByGroup.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("LogicSwitchInteractable: AlertLogicByGroup is empty"));
		return false;
	}

	double CurrentTimeSeconds = GetWorld()->GetTimeSeconds();
	if (CurrentTimeSeconds - LastInteractTimeSeconds < InteractCD)
	{
		return false;
	}

	bool bAllSuccessed = true;
	if (!bSwitchMode)
	{
		for (const TPair < FName, UAIConfigSet*>& Group :SwitchLogicByGroup)
		{
			bool bSuccess = NPCGroupSpawner->SwitchGroupLogic(Group.Key, Group.Value);
			bAllSuccessed = bAllSuccessed && bSuccess;
		}

		if (bAllSuccessed)
		{
			bSwitchMode = true;
			LastInteractTimeSeconds = CurrentTimeSeconds;
			UE_LOG(LogTemp, Log, TEXT("LogicSwitchInteractable: Switch to mode 2"));
			return true;
		}

		UE_LOG(LogTemp, Warning, TEXT("LogicSwitchInteractable: Failed to switch to mode 2"));
		return false;
	}

	for (const TPair < FName, UAIConfigSet*>& Group :SwitchLogicByGroup)
	{
		bool bSuccess = NPCGroupSpawner->RevertDefaultGroupLogic(Group.Key);
		bAllSuccessed = bAllSuccessed && bSuccess;
	}

	if (bAllSuccessed)
	{
		bSwitchMode = false;
		LastInteractTimeSeconds = GetWorld()->GetTimeSeconds();
		UE_LOG(LogTemp, Log, TEXT("LogicSwitchInteractable: Revert to mode 1"));
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("LogicSwitchInteractable: Failed to revert to mode 1"));
	return false;
}

// Called when the game starts or when spawned
void ALogicSwitchInteractable::BeginPlay()
{
	Super::BeginPlay();
	
	BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALogicSwitchInteractable::OnBoxTriggerBeginOverlap);
	BoxTrigger->OnComponentEndOverlap.AddDynamic(this, &ALogicSwitchInteractable::OnBoxTriggerEndOverlap);

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		EnableInput(PC);
		if (InputComponent)
		{
			InputComponent->BindKey(EKeys::E, IE_Pressed, this, &ALogicSwitchInteractable::TryInteract);
		}
	}
}

// Called every frame
void ALogicSwitchInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALogicSwitchInteractable::OnBoxTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IGameplayTagAssetInterface* TagAsset = Cast<IGameplayTagAssetInterface>(OtherActor);
	if (!TagAsset)
	{
		return;
	}

	FGameplayTagContainer ActorTags;
	TagAsset->GetOwnedGameplayTags(ActorTags);
	if (ActorTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Target.Player"))))
	{
		bCanInteract = true;
		CachedOverlappingPlayerPawn = Cast<APawn>(OtherActor);
		UpdateUIText();
	}

}

void ALogicSwitchInteractable::OnBoxTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CachedOverlappingPlayerPawn.IsValid() && OtherActor == CachedOverlappingPlayerPawn.Get())
	{
		bCanInteract = false;
		CachedOverlappingPlayerPawn = nullptr;
		UpdateUIText();
	}
}

void ALogicSwitchInteractable::TryInteract()
{
	if (bCanInteract && CachedOverlappingPlayerPawn.IsValid())
	{
		Interact(CachedOverlappingPlayerPawn.Get());
		UpdateUIText();
	}
}

void ALogicSwitchInteractable::UpdateUIText()
{
	if (!TextComponent)
	{
		return;
	}

	if (!bCanInteract)
	{
		TextComponent->SetHiddenInGame(true);
		return;
	}

	const FString ModeText = bSwitchMode ? TEXT("ON") : TEXT("OFF");
	const FString DisplayText = FString::Printf(TEXT("Press E\nSwitchMode: %s"), *ModeText);

	TextComponent->SetText(FText::FromString(DisplayText));
	TextComponent->SetHiddenInGame(false);
	if (bSwitchMode)
	{
		TextComponent->SetTextRenderColor(FColor::Red);
	}
	else
	{
		TextComponent->SetTextRenderColor(FColor::Green);
	}
}

