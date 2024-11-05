#include "Core/MHGameMode.h"

#include "EngineUtils.h"
#include "MHPlayerCharacter.h"
#include "Audio/MHAudioSubsystem.h"
#include "Core/MHGameState.h"
#include "Core/MHPlayerController.h"
#include "GameFramework/SpectatorPawn.h"
#include "Utils/LLog.h"
#include "Networking/PersistentDataManager.h"
#include "Audio/VoiceChat.h"
#include "Audio/VoiceChatManager.h"
#include "Core/MHGameInstance.h"
#include "Core/MHPlayerState.h"

class AMHGameState;

LL_FILE_CVAR(MHGameMode);

AMHGameMode::AMHGameMode()
{
	bUseSeamlessTravel = true;
}

void AMHGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);

	// Add the PersistentDataManager to the list of actors that will be transferred
	ActorList.Add(PersistentDataManager);

	// Iterate through all AVoiceChat actors in the current world and add them to the list
	// for (TActorIterator<AVoiceChat> It(GetWorld()); It; ++It)
	// {
	// 	AVoiceChat* VoiceChatActor = *It;
	// 	if (VoiceChatActor && VoiceChatActor->IsValidLowLevel())
	// 	{
	// 		ActorList.Add(VoiceChatActor);
	// 		LL_DBG(this, "AMHGameMode::GetSeamlessTravelActorList : Added VoiceChat actor with OdinID ({0}) to the list", VoiceChatActor->OdinID);
	// 	}
	// }
}

void AMHGameMode::InitGameState()
{
	Super::InitGameState();

	// Check if the PersistentDataManager is spawned and assign it in the game state
	for (APersistentDataManager* DataManager : TActorRange<APersistentDataManager>(GetWorld()))
	{
		if (DataManager)
		{
			PersistentDataManager = DataManager; // Reassign the PersistentDataManager
			LL_DBG(this, "AMHGameMode::InitGameState : Found PersistentDataManager, reassigning it");
			
			if(AMHGameState* GS = GetGameState<AMHGameState>())
			{
				GS->PersistentDataManager = DataManager; // Cache it in GameState so it's easily accessible client-side
				LL_DBG(this, "AMHGameMode::InitGameState : Cached PersistentDataManager in GameState");
			}
		}
		else
		{
			LL_ERR(this, "AMHGameMode::InitGameState : Found PersistentDataManager but it was nullptr");
		}
	}
	if (!PersistentDataManager)
	{
		LL_DBG(this, "AMHGameMode::InitGameState : PersistentDataManager was not found (maybe it not spawned yet which is normal then)");
	}

	if (bSpawnVoiceChatManagerOnInitGameState)
	{
		// Spawn the VoiceChatManager
		LL_DBG(this, "AMHGameMode::InitGameState : Spawning VoiceChatManager...");
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		VoiceChatManager = GetWorld()->SpawnActor<AVoiceChatManager>(SpawnParams);
		VoiceChatManager->VoiceChatBlueprint = VoiceChatBlueprint;
		LL_DBG(this, "AMHGameMode::InitGameState : VoiceChatManager spawned");
	}
	else
	{
		LL_DBG(this, "AMHGameMode::InitGameState : bSpawnVoiceChatManagerOnInitGameState is false, not spawning VoiceChatManager");
	}}

void AMHGameMode::PlayerCharacterDied(AController* Controller)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, Controller->GetPawn()->GetActorTransform(), SpawnParameters);

	Controller->UnPossess();
	Controller->Possess(SpectatorPawn);

	FTimerHandle RespawnTimerHandle;

	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &AMHGameMode::RespawnPlayerCharacter,Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, 5, false);

	AMHPlayerController* PC = Cast<AMHPlayerController>(Controller);
	if (PC)
	{
		// TODO : Set UI Callback here
		//PC->SetRespawnCountdown(RespawnDelay);
	}
}

bool AMHGameMode::AllowCheats(APlayerController* P)
{
	return true;
}

// ONLY CALLED ON SERVER
void AMHGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	LL_DBG(this, "AMHGameMode::PostLogin : Login called");
	if (VoiceChatManager.IsValid())
	{
		VoiceChatManager->PostLogin(NewPlayer);
	}
}

void AMHGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (VoiceChatManager.IsValid())
	{
		VoiceChatManager->Logout(Exiting);
	}
}

void AMHGameMode::RespawnPlayerCharacter(AController* Controller)
{
	if (Controller->IsPlayerController())
	{
		// Respawn player
		AActor* PlayerStart = FindPlayerStart(Controller);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AMHPlayerCharacter* PlayerCharacter = GetWorld()->SpawnActor<AMHPlayerCharacter>(PlayerCharacterClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParameters);

		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(PlayerCharacter);
	}
	/*else
	{
		// Respawn AI hero
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AGDHeroCharacter* Hero = GetWorld()->SpawnActor<AGDHeroCharacter>(HeroClass, EnemySpawnPoint->GetActorTransform(), SpawnParameters);
		
		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Hero);
	}*/
}
