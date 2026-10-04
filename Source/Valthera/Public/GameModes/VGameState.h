// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "VGameState.generated.h"

class UVExperienceManagerComponent;
/**
 * 
 */
UCLASS(MinimalAPI, Blueprintable,Config = Game)
class AVGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AVGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	TObjectPtr<UVExperienceManagerComponent> ExperienceManagerComponent;
};
