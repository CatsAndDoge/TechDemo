// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionModule.generated.h"

class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionModuleStateChanged, FName, ModuleName);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TECHDEMO_API UActionModule : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionModule();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	bool CanActivate();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	FName ModuleName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
	int Priority = 0;
	UPROPERTY(BlueprintAssignable, Category = "Action")
	FOnActionModuleStateChanged OnActivated;
	UPROPERTY(BlueprintAssignable, Category = "Action")
	FOnActionModuleStateChanged OnDeactivated;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	ACharacter* CharacterOwner;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
