// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCGroupConfigAsset.h"

bool UNPCGroupConfigAsset::ValidateConfig(FString& OutError) const
{
	if (Groups.Num() == 0)
	{
		OutError = TEXT("At least one group must be defined.");
		return false;
	}
	
	for (const FNPCGroupSpawnRule& Rule : Groups)
	{
		if (Rule.GroupID.IsNone())
		{
			OutError = TEXT("GroupID cannot be None.");
			return false;
		}
		if (!Rule.CharacterClass)
		{
			OutError = FString::Printf(TEXT("CharacterClass must be set for group %s."), *Rule.GroupID.ToString());
			return false;
		}
		if (!Rule.DefaultAIConfig)
		{
			OutError = FString::Printf(TEXT("AIConfig must be set for group %s."), *Rule.GroupID.ToString());
			return false;
		}
		if (Rule.SpawnCount < 1)
		{
			OutError = FString::Printf(TEXT("SpawnCount must be at least 1 for group %s."), *Rule.GroupID.ToString());
			return false;
		}
		if (Rule.MinDistanceBetweenNPC < 0.f)
		{
			OutError = FString::Printf(TEXT("MinDistanceBetweenNPC cannot be negative for group %s."), *Rule.GroupID.ToString());
			return false;
		}
		if (Rule.MaxSpawnRetryCount < 1)
		{
			OutError = FString::Printf(TEXT("MaxSpawnRetryCount must be at least 1 for group %s."), *Rule.GroupID.ToString());
			return false;
		}
	}

	return true;
}
