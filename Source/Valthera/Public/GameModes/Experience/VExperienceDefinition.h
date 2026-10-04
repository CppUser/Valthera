// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Engine/DataAsset.h"
#include "VExperienceDefinition.generated.h"

class UVPawnData;



UCLASS(BlueprintType, Const)
class UVExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UVExperienceDefinition();


	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const UVPawnData> DefaultPawnData;
};
