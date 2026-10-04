// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "VWorldSettings.generated.h"

class UVExperienceDefinition;
/**
 * 
 */
UCLASS(MinimalAPI)
class AVWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	AVWorldSettings(const FObjectInitializer& ObjectInitializer);

	FPrimaryAssetId GetDefaultGameplayExperience() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=GameMode)
	TSoftClassPtr<UVExperienceDefinition> DefaultGameplayExperience;

};
