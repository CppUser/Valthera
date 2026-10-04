// Fill out your copyright notice in the Description page of Project Settings.


#include "System/VAssetManager.h"

UVAssetManager::UVAssetManager()
{
}

UVAssetManager& UVAssetManager::Get()
{
	check(GEngine);

	if (UVAssetManager* Singleton = Cast<UVAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to VAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<UVAssetManager>();
}
