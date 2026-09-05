// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ARPlayerState.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Config = Game)
class AARPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AARPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;

	virtual void CopyProperties(APlayerState* PlayerState) override;
};
