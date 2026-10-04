// Fill out your copyright notice in the Description page of Project Settings.


#include "System/VGameInstance.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Utils/VGameplayTags.h"

UVGameInstance::UVGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UVGameInstance::Init()
{
	Super::Init();
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(VGameplayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(VGameplayTags::InitState_DataAvailable, false, VGameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(VGameplayTags::InitState_DataInitialized, false, VGameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(VGameplayTags::InitState_GameplayReady, false, VGameplayTags::InitState_DataInitialized);
	}
	
}

void UVGameInstance::Shutdown()
{
	Super::Shutdown();
}

int32 UVGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	int32 ReturnVal = Super::AddLocalPlayer(NewPlayer, UserId);
	if (ReturnVal != INDEX_NONE)
	{
		if (!PrimaryPlayer.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("AddLocalPlayer: Set %s to Primary Player"), *NewPlayer->GetName());
			PrimaryPlayer = NewPlayer;
		}
		
	}
	
	return ReturnVal;
}

bool UVGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	if (PrimaryPlayer == ExistingPlayer)
	{
		//TODO: do we want to fall back to another player?
		PrimaryPlayer.Reset();
		UE_LOG(LogTemp, Log, TEXT("RemoveLocalPlayer: Unsetting Primary Player from %s"), *ExistingPlayer->GetName());
	}

	return Super::RemoveLocalPlayer(ExistingPlayer);
}
