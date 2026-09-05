// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ARGameState.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Blueprintable,Config = Game)
class AARGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AARGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
