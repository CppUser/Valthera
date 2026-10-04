// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/VPlayerState.h"

#include "Character/Components/VPawnExtComponent.h"
#include "Components/PlayerStateComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameModes/VGameMode.h"
#include "GameModes/Experience/VExperienceManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Utils/VLogChannels.h"

AVPlayerState::AVPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void AVPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);

	SharedParams.Condition = ELifetimeCondition::COND_SkipOwner;

}

void AVPlayerState::SetPawnData(const UVPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;
	
	ForceNetUpdate();
}

void AVPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);

}

void AVPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		UVExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UVExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void AVPlayerState::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void AVPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void AVPlayerState::Reset()
{
	Super::Reset();

	TArray<UPlayerStateComponent*> ModularComponents;
	GetComponents(ModularComponents);
	for (UPlayerStateComponent* Component : ModularComponents)
	{
		Component->Reset();
	}
}

void AVPlayerState::ClientInitialize(class AController* C)
{
	Super::ClientInitialize(C);

	if (UVPawnExtComponent* PawnExtComp = UVPawnExtComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

void AVPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	TInlineComponentArray<UPlayerStateComponent*> PlayerStateComponents;
	GetComponents(PlayerStateComponents);
	for (UPlayerStateComponent* SourcePSComp : PlayerStateComponents)
	{
		if (UPlayerStateComponent* TargetComp = Cast<UPlayerStateComponent>(static_cast<UObject*>(FindObjectWithOuter(PlayerState, SourcePSComp->GetClass(), SourcePSComp->GetFName()))))
		{
			SourcePSComp->CopyProperties(TargetComp);
		}
	}
}

void AVPlayerState::OnRep_PawnData()
{
}

void AVPlayerState::OnExperienceLoaded(const UVExperienceDefinition* CurrentExperience)
{
	if (AVGameMode* VGameMode = GetWorld()->GetAuthGameMode<AVGameMode>())
	{
		if (const UVPawnData* NewPawnData = VGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogV, Error, TEXT("AVPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}
