// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionModule.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UActionModule::UActionModule()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	CharacterOwner = nullptr;
	// ...
}


// Called when the game starts
void UActionModule::BeginPlay()
{
	Super::BeginPlay();
	CharacterOwner = Cast<ACharacter>(GetOwner());
	// ...
	
}

bool UActionModule::CanActivate_Implementation()
{
	return !IsActive() && CharacterOwner;
}

// Called every frame
void UActionModule::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

