// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionModuleManager.generated.h"


class UActionModule;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TECHDEMO_API UActionModuleManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionModuleManager();

	UFUNCTION(BlueprintCallable, Category = "Module Manager")
	void RegisterModule(UActionModule* Module);
	UFUNCTION(BlueprintCallable, Category = "Module Manager")
	bool ActivateModule(FName ModuleName);
	UFUNCTION(BlueprintCallable, Category = "Module Manager")
	void SwitchModule(FName ModuleName);
	UFUNCTION(BlueprintCallable, Category = "Module Manager")
	UActionModule* GetActiveModule() const { return ActiveModule; }
	UFUNCTION(BlueprintCallable, Category = "Module Manager")
	UActionModule* FindModule(FName ModuleName);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Module Manager")
	UActionModule* ActiveModule;
	UPROPERTY(BlueprintReadOnly, Category = "Module Manager")
	TMap<FName, UActionModule*> RegisteredModules;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Module Manager")
	FName DefaultModuleName;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
