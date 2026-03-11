// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TechDemo/TechDemoCharacter.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "DemoCharacter.generated.h"

class UActionModuleManager;
class UActionModule;

UCLASS()
class TECHDEMO_API ADemoCharacter : public ATechDemoCharacter, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADemoCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action")
	UActionModuleManager* ActionModuleManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	FGameplayTagContainer OwnedTags;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool SwitchActionModule(FName ModuleName);
	UFUNCTION(BlueprintCallable, Category = "Action")
	UActionModule* GetCurrentActionModule() const;


};
