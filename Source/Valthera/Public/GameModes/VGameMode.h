// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VGameMode.generated.h"

class UVPawnData;
class UVExperienceDefinition;
/**
 * 
 */
UCLASS(MinimalAPI, Blueprintable, Config = Game)
class AVGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AVGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "V|Pawn")
	const UVPawnData* GetPawnDataForController(const AController* InController) const;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
protected:
	void OnExperienceLoaded(const UVExperienceDefinition* CurrentExperience);
	bool IsExperienceLoaded() const;

	void LoadExperience();
	void OnExperienceReady(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);
};
