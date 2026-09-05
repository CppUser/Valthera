// Fill out your copyright notice in the Description page of Project Settings.


#include "AzerothReborn/Public/GameModes/ARGameMode.h"

#include "Character/ARCharacter.h"
#include "GameModes/ARGameState.h"
#include "Player/ARPlayerController.h"
#include "Player/ARPlayerState.h"

AARGameMode::AARGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GameStateClass = AARGameState::StaticClass();
	PlayerControllerClass = AARPlayerController::StaticClass();
	PlayerStateClass = AARPlayerState::StaticClass();
	DefaultPawnClass = AARCharacter::StaticClass();
	
}
