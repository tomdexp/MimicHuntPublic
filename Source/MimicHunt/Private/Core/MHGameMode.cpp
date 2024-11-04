#include "Core/MHGameMode.h"

#include "EngineUtils.h"
#include "MHPlayerCharacter.h"
#include "Core/MHGameState.h"
#include "Core/MHPlayerController.h"
#include "GameFramework/SpectatorPawn.h"
#include "Utils/LLog.h"
#include "Networking/PersistentDataManager.h"
#include "Audio/VoiceChat.h"
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
			return;
		}
		LL_ERR(this, "AMHGameMode::InitGameState : Found PersistentDataManager but it was nullptr");
		return;
	}
	LL_DBG(this, "AMHGameMode::InitGameState : PersistentDataManager was not found (maybe it not spawned yet which is normal then)");
}

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
	if (bSetupVoiceChatOnLogin) SetupVoiceChatCoroutine(Cast<AMHPlayerController>(NewPlayer));
}

void AMHGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	// TODO : Handle player logout by destroying the voice chat actor associated with the player
}

UE5Coro::TCoroutine<> AMHGameMode::SetupVoiceChatCoroutine(AMHPlayerController* PlayerController)
{
	// Since PostLogin is called only 1 time per player when they connect
	// We can safely assume 1 voice chat actor per player

	if (!PlayerController)
	{
		LL_ERR(this, "AMHGameMode::SetupVoiceChatCoroutine : PlayerController is nullptr");
		co_return;
	}

	while (!PlayerController->PlayerState)
	{
		LL_DBG(this, "AMHGameMode::SetupVoiceChatCoroutine : Waiting for PlayerState to be set...");
		co_await UE5Coro::Latent::NextTick();
		LL_DBG(this, "AMHGameMode::SetupVoiceChatCoroutine : PlayerState is set");
	}
	
	LL_DBG(this, "AMHGameMode::SetupVoiceChatCoroutine : Spawning voice chat actor for player ({0})", PlayerController->PlayerState->GetPlayerName());
	// Set up spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Set the spawn location and rotation
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	// Spawn the actor
	AVoiceChat* NewVoiceChat = GetWorld()->SpawnActor<AVoiceChat>(VoiceChatBlueprint, SpawnLocation, SpawnRotation, SpawnParams);
	AMHPlayerState* PlayerStateToAssociate = Cast<AMHPlayerState>(PlayerController->PlayerState);
	NewVoiceChat->AssociatedPlayerState = PlayerStateToAssociate;

	// Give the ownership of the voice chat actor to the player controller
	NewVoiceChat->SetOwner(PlayerController);
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
