// Fill out your copyright notice in the Description page of Project Settings.


#include "LogicSwitchInteractable.h"
#include "Components/BoxComponent.h"
#include <Kismet/GameplayStatics.h>
#include "GameplayTagAssetInterface.h"
#include "NPCGroupSpawner.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
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

	/*TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PromptText"));
	TextComponent->SetupAttachment(MeshComponent);
	TextComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 140.0f));
	TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextComponent->SetWorldSize(36.0f);
	TextComponent->SetTextRenderColor(FColor::White);
	TextComponent->SetHiddenInGame(true);*/
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PromptWidgetComp"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetDrawSize(FVector2D(500.f, 200.f));
	WidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 140.f));
	WidgetComponent->SetVisibility(false);
}

bool ALogicSwitchInteractable::Interact(AActor* Interactor)
{
	if (!Interactor || NPCGroups.Num() == 0 || !CachedOverlappingPlayerPawn.IsValid())
	{
		return false;
	}

	if (NPCGroups.Num() == 0)
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
		for (const FLogicSwitchGroup& Group : NPCGroups)
		{
			if (!Group.NPCGroupSpawner)
			{
				bAllSuccessed = false;
				continue;
			}

			for (const TPair<FName, UAIConfigSet*>& Pair : Group.SwitchLogicByGroup)
			{
				bool bSuccess = Group.NPCGroupSpawner->SwitchGroupLogic(Pair.Key, Pair.Value);
				bAllSuccessed = bAllSuccessed && bSuccess;
			}
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

	for (const FLogicSwitchGroup& Group : NPCGroups)
	{
		if (!Group.NPCGroupSpawner)
		{
			bAllSuccessed = false;
			continue;
		}

		for (const TPair<FName, UAIConfigSet*>& Pair : Group.SwitchLogicByGroup)
		{
			bool bSuccess = Group.NPCGroupSpawner->RevertDefaultGroupLogic(Pair.Key);
			bAllSuccessed = bAllSuccessed && bSuccess;
		}
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

	if (WidgetComponent && PromptWidgetClass)
	{
		WidgetComponent->SetWidgetClass(PromptWidgetClass);
		WidgetComponent->InitWidget();
		PromptWidgetInstance = WidgetComponent->GetUserWidgetObject();
	}

	UpdatePromptWidget(false, false);

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
		UpdatePromptWidget(true, bSwitchMode);
	}

}

void ALogicSwitchInteractable::OnBoxTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CachedOverlappingPlayerPawn.IsValid() && OtherActor == CachedOverlappingPlayerPawn.Get())
	{
		bCanInteract = false;
		CachedOverlappingPlayerPawn = nullptr;
		UpdateUIText();
		UpdatePromptWidget(false, bSwitchMode);
	}
}

void ALogicSwitchInteractable::TryInteract()
{
	if (bCanInteract && CachedOverlappingPlayerPawn.IsValid())
	{
		Interact(CachedOverlappingPlayerPawn.Get());
		UpdateUIText();
		UpdatePromptWidget(true, bSwitchMode);
	}
}

void ALogicSwitchInteractable::UpdateUIText()
{
	/*if (!TextComponent)
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
	}*/
}

void ALogicSwitchInteractable::UpdatePromptWidget(bool bVisible, bool bSwitchOn)
{
	if (!WidgetComponent)
	{
		return;
	}

	WidgetComponent->SetVisibility(bVisible);

	if (!PromptWidgetInstance || !PromptWidgetInstance->WidgetTree)
	{
		return;
	}

	//if (UTextBlock* ActionText = Cast<UTextBlock>(
	//	PromptWidgetInstance->WidgetTree->FindWidget(TEXT("TxtAction"))))
	//{
	//	ActionText->SetText(FText::FromString(TEXT("Press E")));
	//}

	if (UTextBlock* SwitchText = Cast<UTextBlock>(
		PromptWidgetInstance->WidgetTree->FindWidget(TEXT("SwitchModText"))))
	{
		SwitchText->SetText(FText::FromString(
			bSwitchOn ? TEXT("SwitchMode: ON") : TEXT("SwitchMode: OFF")));
	}
}