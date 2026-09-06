// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/VPlayerController.h"
#include "Components/ControllerComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Player/VLocalPlayer.h"

AVPlayerController::AVPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void AVPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);

}

void AVPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AVPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void AVPlayerController::ReceivedPlayer()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::ReceivedPlayer();

	TArray<UControllerComponent*> ModularComponents;
	GetComponents(ModularComponents);
	for (UControllerComponent* Component : ModularComponents)
	{
		Component->ReceivedPlayer();
	}

	if (UVLocalPlayer* LocalPlayer = Cast<UVLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerControllerSet.Broadcast(LocalPlayer, this);

		if (PlayerState)
		{
			LocalPlayer->OnPlayerStateSet.Broadcast(LocalPlayer, PlayerState);
		}
	}
}

void AVPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	TArray<UControllerComponent*> ModularComponents;
	GetComponents(ModularComponents);
	for (UControllerComponent* Component : ModularComponents)
	{
		Component->PlayerTick(DeltaTime);
	}
}

void AVPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (UVLocalPlayer* LocalPlayer = Cast<UVLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, InPawn);
	}
}

void AVPlayerController::OnPossess(class APawn* APawn)
{
	Super::OnPossess(APawn);

	if (UVLocalPlayer* LocalPlayer = Cast<UVLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, APawn);
	}
}

void AVPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (UVLocalPlayer* LocalPlayer = Cast<UVLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerPawnSet.Broadcast(LocalPlayer, nullptr);
	}
}

void AVPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (PlayerState)
	{
		if (UVLocalPlayer* LocalPlayer = Cast<UVLocalPlayer>(Player))
		{
			LocalPlayer->OnPlayerStateSet.Broadcast(LocalPlayer, PlayerState);
		}
	}
}

void AVPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
