// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIConfigSet.h"
#include "LogicSwitchInteractable.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
//class UTextRenderComponent;
class UWidgetComponent;
class UUserWidget;
class UTextBlock;
class ANPCGroupSpawner;
class APawn;

USTRUCT(BlueprintType)
struct FLogicSwitchGroup
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LogicSwitch")
	ANPCGroupSpawner* NPCGroupSpawner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LogicSwitch")
	TMap<FName, UAIConfigSet*> SwitchLogicByGroup;
};

UCLASS(BlueprintType)
class TECHDEMO_API ALogicSwitchInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALogicSwitchInteractable();
	UFUNCTION(BlueprintCallable, Category = "LogicSwitch")
	bool Interact(AActor* Interactor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* MeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UBoxComponent* BoxTrigger;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UTextRenderComponent* TextComponent;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* WidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> PromptWidgetClass;
	UPROPERTY()
	UUserWidget* PromptWidgetInstance = nullptr;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LogicSwitch")
	ANPCGroupSpawner* NPCGroupSpawner;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LogicSwitch")
	TMap<FName, UAIConfigSet*> SwitchLogicByGroup;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LogicSwitch")
	TArray<FLogicSwitchGroup> NPCGroups;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LogicSwitch")
	float InteractCD = 0.5f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(VisibleInstanceOnly, Category = "LogicSwitch")
	bool bSwitchMode = false;
	UPROPERTY(VisibleInstanceOnly, Category = "LogicSwitch")
	bool bCanInteract = false;
	UPROPERTY(VisibleInstanceOnly, Category = "LogicSwitch")
	TWeakObjectPtr<APawn> CachedOverlappingPlayerPawn;
	double LastInteractTimeSeconds = -1000.0;
	
	UFUNCTION()
	void OnBoxTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
		const FHitResult& SweepResult);
	UFUNCTION()
	void OnBoxTriggerEndOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void TryInteract();
	void UpdateUIText();
	void UpdatePromptWidget(bool bVisible, bool bSwitchOn);
};
