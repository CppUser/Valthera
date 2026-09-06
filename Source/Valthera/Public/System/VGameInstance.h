// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VGameInstance.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Config = Game)
class UVGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UVGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
protected:
	virtual void Init() override;
	virtual void Shutdown() override;
	
	virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId) override;
	virtual bool RemoveLocalPlayer(ULocalPlayer* ExistingPlayer) override;
private:
	TWeakObjectPtr<ULocalPlayer> PrimaryPlayer;
};
