// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/VGameMode.h"

#include "Character/VCharacter.h"
#include "GameModes/VGameState.h"
#include "Player/VPlayerController.h"
#include "Player/VPlayerState.h"

AVGameMode::AVGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GameStateClass = AVGameState::StaticClass();
	PlayerControllerClass = AVPlayerController::StaticClass();
	PlayerStateClass = AVPlayerState::StaticClass();
	DefaultPawnClass = AVCharacter::StaticClass();
	
}
