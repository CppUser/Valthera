// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "VAssetManager.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Config = Game)
class UVAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	UVAssetManager();
	
	static UVAssetManager& Get();
};
