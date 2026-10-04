// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/VPawnExtComponent.h"
#include  "Character/VPawnData.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Net/UnrealNetwork.h"
#include "Utils/VGameplayTags.h"

const FName UVPawnExtComponent::NAME_ActorFeatureName("PawnExtension");

UVPawnExtComponent::UVPawnExtComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	PawnData = nullptr;
}

void UVPawnExtComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UVPawnExtComponent, PawnData);
}

void UVPawnExtComponent::SetPawnData(const UVPawnData* InPawnData)
{
	check(InPawnData);

	APawn* Pawn = GetPawnChecked<APawn>();

	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	Pawn->ForceNetUpdate();

	CheckDefaultInitialization();
}

bool UVPawnExtComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	if (!CurrentState.IsValid() && DesiredState == VGameplayTags::InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}
	if (CurrentState == VGameplayTags::InitState_Spawned && DesiredState == VGameplayTags::InitState_DataAvailable)
	{
		// Pawn data is required.
		if (!PawnData)
		{
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			// Check for being possessed by a controller.
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == VGameplayTags::InitState_DataAvailable && DesiredState == VGameplayTags::InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		return Manager->HaveAllFeaturesReachedInitState(Pawn, VGameplayTags::InitState_DataAvailable);
	}
	else if (CurrentState == VGameplayTags::InitState_DataInitialized && DesiredState == VGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UVPawnExtComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	if (DesiredState == VGameplayTags::InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
	}
}

void UVPawnExtComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == VGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UVPawnExtComponent::CheckDefaultInitialization()
{
	CheckDefaultInitializationForImplementers();

	static const TArray<FGameplayTag> StateChain = { VGameplayTags::InitState_Spawned, VGameplayTags::InitState_DataAvailable, VGameplayTags::InitState_DataInitialized, VGameplayTags::InitState_GameplayReady };

	ContinueInitStateChain(StateChain);
}

void UVPawnExtComponent::HandleControllerChanged()
{
	CheckDefaultInitialization();
}

void UVPawnExtComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UVPawnExtComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void UVPawnExtComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("VPawnExtComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UVPawnExtComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one VPawnExtComponent should exist on [%s]."), *GetNameSafe(GetOwner()));
	
	RegisterInitStateFeature();
}

void UVPawnExtComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
	
	// Notifies state manager that we have spawned, then try rest of default initialization
	ensure(TryToChangeInitState(VGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UVPawnExtComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

void UVPawnExtComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}
