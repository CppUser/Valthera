// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/GameStateComponent.h"
#include "VExperienceManagerComponent.generated.h"


class UVExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnExperienceLoaded, const UVExperienceDefinition* /*Experience*/);


enum class EVExperienceLoadState
{
	Unloaded,
	Loading,
	Loaded
};

UCLASS(MinimalAPI)
class UVExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
public:
	UVExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SetCurrentExperience(FPrimaryAssetId ExperienceId);
	const UVExperienceDefinition* GetCurrentExperienceChecked() const;
	bool IsExperienceLoaded() const;

	void CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate&& Delegate);

protected:
	UFUNCTION()
	void OnRep_CurrentExperience();

	void StartExperienceLoad();
	void OnExperienceLoadComplete();
private:
	UPROPERTY(ReplicatedUsing=OnRep_CurrentExperience)
	TObjectPtr<const UVExperienceDefinition> CurrentExperience;

	EVExperienceLoadState LoadState = EVExperienceLoadState::Unloaded;

	FOnExperienceLoaded OnExperienceLoaded;
};
