// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VGameMode.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Blueprintable, Config = Game)
class AVGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AVGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
