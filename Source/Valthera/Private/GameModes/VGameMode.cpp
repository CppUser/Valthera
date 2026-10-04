// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/VGameMode.h"
#include  "Character/VPawnData.h"
#include "Character/VCharacter.h"
#include "Character/Components/VPawnExtComponent.h"
#include "GameModes/VGameState.h"
#include "GameModes/VWorldSettings.h"
#include "GameModes/Experience/VExperienceDefinition.h"
#include "GameModes/Experience/VExperienceManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/VPlayerController.h"
#include "Player/VPlayerState.h"
#include "System/VAssetManager.h"
#include "Utils/VLogChannels.h"

AVGameMode::AVGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GameStateClass = AVGameState::StaticClass();
	PlayerControllerClass = AVPlayerController::StaticClass();
	PlayerStateClass = AVPlayerState::StaticClass();
	DefaultPawnClass = AVCharacter::StaticClass();
	
}

const UVPawnData* AVGameMode::GetPawnDataForController(const AController* InController) const
{
	if (InController != nullptr)
	{
		if (const AVPlayerState* PS = InController->GetPlayerState<AVPlayerState>())
		{
			if (const UVPawnData* PawnData = PS->GetPawnData<UVPawnData>())
			{
				return PawnData;
			}
		}
	}
	check(GameState);
	UVExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UVExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded())
	{
		const UVExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;
		}

		//TODO: Experience is loaded and there's still no pawn data, fall back to the default for now
		//return UVAssetManager::Get().GetDefaultPawnData();
	}
	return nullptr;
}

UClass* AVGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UVPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* AVGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UVPawnExtComponent* PawnExtComp = UVPawnExtComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UVPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogV, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogV, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogV, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

void AVGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

void AVGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::LoadExperience);
}

void AVGameMode::InitGameState()
{
	Super::InitGameState();

	UVExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UVExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void AVGameMode::OnExperienceLoaded(const UVExperienceDefinition* CurrentExperience)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

bool AVGameMode::IsExperienceLoaded() const
{
	check(GameState);
	UVExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UVExperienceManagerComponent>();
	check(ExperienceComponent);

	return ExperienceComponent->IsExperienceLoaded();
}

void AVGameMode::LoadExperience()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	// Precedence order (highest wins)
	//  - Matchmaking assignment (if present)
	//  - URL Options override
	//  - Developer Settings (PIE only)
	//  - Command Line override
	//  - World Settings
	//  - Dedicated server
	//  - Default experience

	UWorld* World = GetWorld();

	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UVExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}

	//TODO: Need to be able to load from developer settings
	// if (!ExperienceId.IsValid() && World->IsPlayInEditor())
	// {
	// 	ExperienceId = GetDefault<UVDeveloperSettings>()->ExperienceOverride;
	// 	ExperienceIdSource = TEXT("DeveloperSettings");
	// }

	// see if the command line wants to set the experience
	if (!ExperienceId.IsValid())
	{
		FString ExperienceFromCommandLine;
		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
		{
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
			if (!ExperienceId.PrimaryAssetType.IsValid())
			{
				ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UVExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromCommandLine));
			}
			ExperienceIdSource = TEXT("CommandLine");
		}
	}

	// see if the world settings has a default experience
	if (!ExperienceId.IsValid())
	{
		if (AVWorldSettings* TypedWorldSettings = Cast<AVWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	UVAssetManager& AssetManager = UVAssetManager::Get();
	FAssetData Dummy;
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		UE_LOG(LogVExperience, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();
	}

	// Final fallback to the default experience
	if (!ExperienceId.IsValid())
	{
		// TODO: if (TryDedicatedServerLogin())
		// {
		// 	// This will start to host as a dedicated server
		// 	return;
		// }

		//@TODO: Pull this from a config setting or something
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("VExperienceDefinition"), FName("B_TestExperience"));
		ExperienceIdSource = TEXT("Default");
	}

	OnExperienceReady(ExperienceId, ExperienceIdSource);
}

void AVGameMode::OnExperienceReady(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogVExperience, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);

		UVExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UVExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->SetCurrentExperience(ExperienceId);
	}
	else
	{
		UE_LOG(LogVExperience, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
}

