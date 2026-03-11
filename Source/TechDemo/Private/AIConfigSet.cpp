// Fill out your copyright notice in the Description page of Project Settings.


#include "AIConfigSet.h"

bool UAIConfigSet::ValidateConfig(FString& OutError)
{
	if (!BehaviorTree)
	{
		OutError = FString::Printf(TEXT("AI Config %s: BehaviorTree is not assigned."), *ConfigID.ToString());
		return false;
	}

	if (!BlackboardData)
	{
		OutError = FString::Printf(TEXT("AI Config %s: BlackboardData is not assigned."), *ConfigID.ToString());
		return false;
	}

	if (PerceptionConfig.bEnableSight)
	{
		if (PerceptionConfig.SightRadius <= 0)
		{
			OutError = FString::Printf(TEXT("AI Config %s: SightRadius must be greater than 0."), *ConfigID.ToString());
			return false;
		}
		if (PerceptionConfig.LoseSightRadius <= PerceptionConfig.SightRadius)
		{
			OutError = FString::Printf(TEXT("AI Config %s: LoseSightRadius must be greater than SightRadius."), *ConfigID.ToString());
			return false;
		}
		if (PerceptionConfig.PeripheralVisionAngleDegrees < 0 || PerceptionConfig.PeripheralVisionAngleDegrees > 180)
		{
			OutError = FString::Printf(TEXT("AI Config %s: PeripheralVisionAngleDegrees must be between 0 and 180."), *ConfigID.ToString());
			return false;
		}
		if (PerceptionConfig.MaxAge < 0)
		{
			OutError = FString::Printf(TEXT("AI Config %s: MaxAge must be greater than or equal to 0."), *ConfigID.ToString());
			return false;
		}
	}

	if (MovementConfig.WalkSpeed < 0)
	{
		OutError = FString::Printf(TEXT("AI Config %s: WalkSpeed must be greater than or equal to 0."), *ConfigID.ToString());
		return false;
	}

	if (MovementConfig.SprintSpeed < MovementConfig.WalkSpeed)
	{
		OutError = FString::Printf(TEXT("AI Config %s: SprintSpeed must be greater than or equal to WalkSpeed."), *ConfigID.ToString());
		return false;
	}

	if (bRequireAnyAllowedTag && AllowedTargetTags.Num() == 0)
	{
		OutError = FString::Printf(TEXT("AI Config %s: AllowedTargetTags must contain at least one tag if bRequireAnyAllowedTag is true."), *ConfigID.ToString());
		return false;
	}

	return true;
}