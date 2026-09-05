// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ARGameMode.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Blueprintable, Config = Game)
class AARGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AARGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
