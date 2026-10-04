// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VPawnData.generated.h"

/**
 * 
 */
UCLASS()
class VALTHERA_API UVPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "V|Pawn")
	TSubclassOf<APawn> PawnClass;
};
