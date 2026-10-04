// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include  "Character/VPawnData.h"
#include "VPlayerState.generated.h"

class UVExperienceDefinition;
class UVPawnData;
/**
 * 
 */
UCLASS(MinimalAPI, Config = Game)
class AVPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AVPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UVPawnData* InPawnData);
protected:
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;

	virtual void ClientInitialize(class AController* C) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;

	UFUNCTION()
	void OnRep_PawnData();
private:
	void OnExperienceLoaded(const UVExperienceDefinition* CurrentExperience);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UVPawnData> PawnData;
};

