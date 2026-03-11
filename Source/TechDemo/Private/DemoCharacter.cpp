// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActionModuleManager.h"
#include "ActionModule.h"

// Sets default values
ADemoCharacter::ADemoCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::PlacedInWorld;

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxAcceleration = 2048.f;

	ActionModuleManager = CreateDefaultSubobject<UActionModuleManager>(TEXT("ActionModuleManager"));

}

// Called when the game starts or when spawned
void ADemoCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("DemoCharacter BeginPlay"));
	
}

// Called every frame
void ADemoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADemoCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = OwnedTags;
}

bool ADemoCharacter::SwitchActionModule(FName ModuleName)
{
	if (ActionModuleManager)
	{
		ActionModuleManager->SwitchModule(ModuleName);
		return true;
	}
	return false;
}

UActionModule* ADemoCharacter::GetCurrentActionModule() const
{
	if (ActionModuleManager)
	{
		return ActionModuleManager->GetActiveModule();
	}
	return nullptr;
}