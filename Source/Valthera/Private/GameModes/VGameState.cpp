// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/VGameState.h"

#include "Components/GameFrameworkComponentManager.h"
#include "GameModes/Experience/VExperienceManagerComponent.h"

AVGameState::AVGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	ExperienceManagerComponent = ObjectInitializer.CreateDefaultSubobject<UVExperienceManagerComponent>(this,TEXT("ExperienceManagerComponent"));

}

void AVGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);

}

void AVGameState::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void AVGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}
