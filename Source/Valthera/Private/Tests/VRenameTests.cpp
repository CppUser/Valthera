// Regression coverage for native class references, Blueprint parents, and game startup.

#if WITH_EDITOR && WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Character/VCharacter.h"
#include "GameModes/VGameMode.h"
#include "GameModes/VGameState.h"
#include "Input/VInputComponent.h"
#include "Input/VInputConfig.h"
#include "Player/VLocalPlayer.h"
#include "Player/VPlayerController.h"
#include "Player/VPlayerState.h"
#include "System/VGameInstance.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "GameMapsSettings.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Tests/AutomationEditorCommon.h"
#include "Tests/AutomationCommon.h"
#include "UObject/CoreRedirects.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVClassRedirectsTest, "Project.Valthera.Rename.ClassRedirects",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVClassRedirectsTest::RunTest(const FString& Parameters)
{
	UClass* Classes[] = {
		AVCharacter::StaticClass(), AVGameMode::StaticClass(), AVGameState::StaticClass(),
		UVInputComponent::StaticClass(), UVInputConfig::StaticClass(), UVLocalPlayer::StaticClass(),
		AVPlayerController::StaticClass(), AVPlayerState::StaticClass(), UVGameInstance::StaticClass()
	};
	for (UClass* Class : Classes)
	{
		const FString OldPath = TEXT("/Script/AzerothReborn.AR") + Class->GetName().RightChop(1);
		const FCoreRedirectObjectName Redirected = FCoreRedirects::GetRedirectedName(
			ECoreRedirectFlags::Type_Class, FCoreRedirectObjectName(OldPath));
		TestEqual(OldPath, Redirected.ToString(), Class->GetPathName());
		TestEqual(TEXT("Class belongs to Valthera"), Class->GetOutermost()->GetName(), FString(TEXT("/Script/Valthera")));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVGameDefaultsTest, "Project.Valthera.Rename.GameDefaults",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVGameDefaultsTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("Configured game mode"), UGameMapsSettings::GetGlobalDefaultGameMode(), AVGameMode::StaticClass()->GetPathName());
	TestEqual(TEXT("Configured game instance"), GetDefault<UGameMapsSettings>()->GameInstanceClass.TryLoadClass<UGameInstance>(), UVGameInstance::StaticClass());
	TestEqual(TEXT("Configured local player"), GEngine->LocalPlayerClass.Get(), UVLocalPlayer::StaticClass());
	const AVGameMode* GameMode = GetDefault<AVGameMode>();
	TestEqual(TEXT("Default pawn"), GameMode->DefaultPawnClass.Get(), AVCharacter::StaticClass());
	TestEqual(TEXT("Game state"), GameMode->GameStateClass.Get(), AVGameState::StaticClass());
	TestEqual(TEXT("Player controller"), GameMode->PlayerControllerClass.Get(), AVPlayerController::StaticClass());
	TestEqual(TEXT("Player state"), GameMode->PlayerStateClass.Get(), AVPlayerState::StaticClass());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVBlueprintParentsTest, "Project.Valthera.Rename.BlueprintParents",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVBlueprintParentsTest::RunTest(const FString& Parameters)
{
	struct FExpectedBlueprint
	{
		const TCHAR* Path;
		UClass* NativeParent;
	};
	const FExpectedBlueprint Blueprints[] = {
		{ TEXT("/Game/Blueprints/Character/B_CharacterBase.B_CharacterBase"), AVCharacter::StaticClass() },
		{ TEXT("/Game/Blueprints/Character/Heroes/Alliance/Human/Mage/B_Hum_Mage.B_Hum_Mage"), AVCharacter::StaticClass() },
		{ TEXT("/Game/Blueprints/GameModes/B_GameModeBase.B_GameModeBase"), AVGameMode::StaticClass() }
	};
	for (const FExpectedBlueprint& Expected : Blueprints)
	{
		UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, Expected.Path);
		if (!TestNotNull(Expected.Path, Blueprint))
		{
			continue;
		}
		FKismetEditorUtilities::CompileBlueprint(Blueprint);
		TestTrue(TEXT("Blueprint compiles"), Blueprint->Status != BS_Error);
		if (TestNotNull(TEXT("Generated class"), Blueprint->GeneratedClass.Get()))
		{
			TestTrue(Expected.Path, Blueprint->GeneratedClass->IsChildOf(Expected.NativeParent));
		}
	}
	UClass* GameModeClass = LoadClass<AGameModeBase>(nullptr, TEXT("/Game/Blueprints/GameModes/B_GameModeBase.B_GameModeBase_C"));
	UClass* PawnClass = LoadClass<APawn>(nullptr, TEXT("/Game/Blueprints/Character/Heroes/Alliance/Human/Mage/B_Hum_Mage.B_Hum_Mage_C"));
	if (TestNotNull(TEXT("Blueprint game mode class"), GameModeClass) && TestNotNull(TEXT("Blueprint pawn class"), PawnClass))
	{
		TestEqual(TEXT("Blueprint game mode retains its chosen pawn"), GameModeClass->GetDefaultObject<AGameModeBase>()->DefaultPawnClass.Get(), PawnClass);
	}
	return true;
}

class FVVerifyPlaySession : public IAutomationLatentCommand
{
public:
	explicit FVVerifyPlaySession(FAutomationTestBase* InTest) : Test(InTest) {}

	virtual bool Update() override
	{
		if (StartTime == 0.0)
		{
			StartTime = FPlatformTime::Seconds();
		}
		UWorld* World = GEditor->PlayWorld;
		APlayerController* Controller = World ? World->GetFirstPlayerController() : nullptr;
		if (!World || !World->HasBegunPlay() || !Controller || !Controller->GetPawn())
		{
			if (FPlatformTime::Seconds() - StartTime > 30.0)
			{
				Test->AddError(TEXT("Play in Editor did not start with a possessed pawn within 30 seconds."));
				return true;
			}
			return false;
		}
		Test->TestNotNull(TEXT("Live Valthera game instance"), Cast<UVGameInstance>(World->GetGameInstance()));
		Test->TestNotNull(TEXT("Live Valthera game mode"), Cast<AVGameMode>(World->GetAuthGameMode()));
		Test->TestNotNull(TEXT("Live Valthera game state"), Cast<AVGameState>(World->GetGameState()));
		Test->TestNotNull(TEXT("Live Valthera player controller"), Cast<AVPlayerController>(Controller));
		Test->TestNotNull(TEXT("Live Valthera player state"), Cast<AVPlayerState>(Controller->PlayerState));
		Test->TestNotNull(TEXT("Live Valthera local player"), Cast<UVLocalPlayer>(Controller->GetLocalPlayer()));
		Test->TestNotNull(TEXT("Possessed Valthera character"), Cast<AVCharacter>(Controller->GetPawn()));
		return true;
	}

private:
	FAutomationTestBase* Test;
	double StartTime = 0.0;
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVPlayInEditorTest, "Project.Valthera.Rename.PlayInEditor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVPlayInEditorTest::RunTest(const FString& Parameters)
{
	ADD_LATENT_AUTOMATION_COMMAND(FEditorLoadMap(UGameMapsSettings::GetGameDefaultMap()));
	ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
	ADD_LATENT_AUTOMATION_COMMAND(FVVerifyPlaySession(this));
	ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand());
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.0f));
	return true;
}

#endif
