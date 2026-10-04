// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "VPawnExtComponent.generated.h"

class UVPawnData;
/**
 * 
 */
UCLASS(MinimalAPI)
class UVPawnExtComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UVPawnExtComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "V|Pawn")
	static UVPawnExtComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UVPawnExtComponent>() : nullptr); }
	
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UVPawnData* InPawnData);

	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;

	void HandleControllerChanged();
	void HandlePlayerStateReplicated();
	void SetupPlayerInputComponent();
protected:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnRep_PawnData();
public:
	static const FName NAME_ActorFeatureName;
protected:
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "V|Pawn")
	TObjectPtr<const UVPawnData> PawnData;
};
