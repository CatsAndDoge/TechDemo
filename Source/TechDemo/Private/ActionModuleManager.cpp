// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionModuleManager.h"
#include "ActionModule.h"

// Sets default values for this component's properties
UActionModuleManager::UActionModuleManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UActionModuleManager::BeginPlay()
{
	Super::BeginPlay();
	TArray<UActionModule*> Modules;
	GetOwner()->GetComponents<UActionModule>(Modules);
	for (UActionModule* Module : Modules)
	{
		if (Module)
		{
			RegisterModule(Module);
		}
	}
	
	if (DefaultModuleName.IsValid())
	{
		ActivateModule(DefaultModuleName);
	}
	
}

void UActionModuleManager::RegisterModule(UActionModule* Module)
{
	if (Module)
	{
		RegisteredModules.Add(Module->ModuleName, Module);
		UE_LOG(LogTemp, Log, TEXT("Module %s registered"), *Module->ModuleName.ToString());
	}
}

bool UActionModuleManager::ActivateModule(FName ModuleName)
{
	UActionModule* Module = FindModule(ModuleName);
	if (Module && Module->CanActivate())
	{
		Module->Activate();
		return true;
	}
	
	return false;
}

void UActionModuleManager::SwitchModule(FName ModuleName)
{
	if (ActiveModule && ActiveModule->ModuleName == ModuleName) return;
	if (ActiveModule)
	{
		ActiveModule->Deactivate();
	}
	
	UActionModule* NewModule = FindModule(ModuleName);
	if (NewModule && NewModule->CanActivate())
	{
		NewModule->Activate();
		ActiveModule = NewModule;
	}
}

UActionModule* UActionModuleManager::FindModule(FName ModuleName)
{
	return RegisteredModules.FindRef(ModuleName);
}


// Called every frame
void UActionModuleManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

