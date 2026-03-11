// Fill out your copyright notice in the Description page of Project Settings.


#include "TechDemoPlayerController.h"
#include "TechDemoCheatManager.h"

ATechDemoPlayerController::ATechDemoPlayerController()
{
	CheatClass = UTechDemoCheatManager::StaticClass();
}
