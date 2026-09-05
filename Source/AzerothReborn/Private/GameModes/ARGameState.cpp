// Fill out your copyright notice in the Description page of Project Settings.


#include "AzerothReborn/Public/GameModes/ARGameState.h"

#include "Components/GameFrameworkComponentManager.h"

AARGameState::AARGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void AARGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);

}

void AARGameState::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void AARGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}
