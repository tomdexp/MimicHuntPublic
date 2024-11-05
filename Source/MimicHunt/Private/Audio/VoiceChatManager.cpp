#include "Audio/VoiceChatManager.h"

#include "EngineUtils.h"
#include "Audio/MHAudioSubsystem.h"
#include "Core/MHPlayerController.h"
#include "Core/MHPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(VoiceChatManager);

AVoiceChatManager::AVoiceChatManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVoiceChatManager::BeginPlay()
{
	Super::BeginPlay();
	if (!VoiceChatBlueprint)
	{
		LL_WRN(this, "AVoiceChatManager::BeginPlay : VoiceChatBlueprint is nullptr, cannot spawn voice chat actors");
	}
}

void AVoiceChatManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Search for all present player states in the scene
	// If a player state with a valid Odin ID is found and the voice chat actor is not spawned, spawn it
	if (AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>())
	{
		for (APlayerState* PlayerState : GameState->PlayerArray)
		{
			if (AMHPlayerState* MHPlayerState = Cast<AMHPlayerState>(PlayerState))
			{
				if (MHPlayerState->OdinID != FGuid())
				{
					bool bFound = false;
					for (TWeakObjectPtr<AVoiceChat> VoiceChatActor : SpawnedVoiceChatActors)
					{
						if (VoiceChatActor.IsValid() && VoiceChatActor->OdinID == MHPlayerState->OdinID)
						{
							bFound = true;
							break;
						}
					}
					
					if (!bFound)
					{
						LL_DBG(this, "AVoiceChatManager::Tick : Found player state with Odin ID {0} that has no voice chat actor, spawning voice chat actor", MHPlayerState->OdinID);
						CreateVoiceChatForPlayer(MHPlayerState->OdinID);
					}
				}
			}
		}
	}
}

void AVoiceChatManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	LL_DBG(this, "AVoiceChatManager::EndPlay : Destroying voice chat manager");
	// Destroy all voice chat actors
	for (TWeakObjectPtr<AVoiceChat> VoiceChatActor : SpawnedVoiceChatActors)
	{
		if (VoiceChatActor.IsValid())
		{
			LL_DBG(this, "AVoiceChatManager::EndPlay : Destroying voice chat actor with Odin ID {0}", VoiceChatActor->OdinID);
			VoiceChatActor->Destroy();
		}
	}
}

void AVoiceChatManager::PostLogin(APlayerController* NewPlayer)
{
	LL_DBG(this, "AVoiceChatManager::PostLogin : Player logged in for the first time, spawning voice chat actor");
	CreateOdinIDForPlayerCoroutine(Cast<AMHPlayerController>(NewPlayer));
}

void AVoiceChatManager::Logout(AController* Exiting)
{
	LL_DBG(this, "AVoiceChatManager::Logout : Player logged out, destroying voice chat actor");
}

void AVoiceChatManager::CreateVoiceChatForPlayer(FGuid OdinID)
{
	// Since PostLogin is called only 1 time per player when they connect
	// We can safely assume 1 voice chat actor per playe

	if (!VoiceChatBlueprint)
	{
		LL_ERR(this, "AVoiceChatManager::SetupVoiceChatCoroutine : VoiceChatBlueprint is nullptr, cannot spawn voice chat actor");
		return;
	}

	// Check that this Odin ID is not already in process
	if (OdinIDsInProcess.Contains(OdinID))
	{
		LL_DBG(this, "AVoiceChatManager::CreateVoiceChatForPlayerCoroutine : Odin ID {0} is already in process", OdinID);
		return;
	}

	// Add the Odin ID to the list of Odin IDs in process
	OdinIDsInProcess.Add(OdinID);
	
	// Iterate through all player states to find the player state with the Odin ID
	AMHPlayerState* PlayerState = nullptr;
	for (TActorIterator<AMHPlayerState> It(GetWorld()); It; ++It)
	{
		AMHPlayerState* CurrentPlayerState = *It;
		if (CurrentPlayerState && CurrentPlayerState->OdinID == OdinID)
		{
			PlayerState = CurrentPlayerState;
			break;
		}
	}

	if (!PlayerState)
	{
		LL_WRN(this, "AVoiceChatManager::CreateVoiceChatForPlayerCoroutine : Could not find player state with Odin ID {0}", OdinID);
	}
	
	LL_DBG(this, "AVoiceChatManager::CreateVoiceChatForPlayerCoroutine : Spawning voice chat actor for player ({0})", PlayerState->GetPlayerName());
	
	// Set up spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Set the spawn location and rotation
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	// Spawn the actor
	AVoiceChat* NewVoiceChat = GetWorld()->SpawnActor<AVoiceChat>(VoiceChatBlueprint, SpawnLocation, SpawnRotation, SpawnParams);
	AMHPlayerState* PlayerStateToAssociate = Cast<AMHPlayerState>(PlayerState);

	// Fill the necessary properties of voice chat
	NewVoiceChat->OdinID = OdinID;
	NewVoiceChat->OnRep_OdinID();
	NewVoiceChat->AssociatedPlayerState = PlayerStateToAssociate;
	
	SpawnedVoiceChatActors.Add(NewVoiceChat);

	// Remove the Odin ID from the list of Odin IDs in process
	OdinIDsInProcess.Remove(OdinID);
}

UE5Coro::TCoroutine<> AVoiceChatManager::CreateOdinIDForPlayerCoroutine(AMHPlayerController* PlayerController)
{
	if (!PlayerController)
	{
		LL_ERR(this, "AVoiceChatManager::CreateOdinIDForPlayerCoroutine : PlayerController is nullptr");
		co_return;
	}

	while (!PlayerController->PlayerState)
	{
		LL_DBG(this, "AVoiceChatManager::CreateOdinIDForPlayerCoroutine : Waiting for PlayerState to be set...");
		co_await UE5Coro::Latent::NextTick();
		LL_DBG(this, "AVoiceChatManager::CreateOdinIDForPlayerCoroutine : PlayerState is set");
	}

	// Cast to MHPlayerState
	AMHPlayerState* PlayerState = Cast<AMHPlayerState>(PlayerController->PlayerState);
	
	FGuid NewOdinID = FGuid::NewGuid();
	PlayerState->OdinID = NewOdinID;
	LL_DBG(this, "AVoiceChatManager::CreateOdinIDForPlayerCoroutine : Created Odin ID {0} for player {1}", NewOdinID, PlayerState->GetPlayerName());

	// Get the MHAudioSubsystem
	if (UMHAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UMHAudioSubsystem>())
	{
		AudioSubsystem->PlayerOdinIDCreated(NewOdinID);
	}
}

void AVoiceChatManager::DeleteOdinIDForPlayer(AMHPlayerController* PlayerController)
{
	if (!PlayerController)
	{
		LL_ERR(this, "AVoiceChatManager::DeleteOdinIDForPlayer : PlayerController is nullptr");
		return;
	}

	// Cast to MHPlayerState
	AMHPlayerState* PlayerState = Cast<AMHPlayerState>(PlayerController->PlayerState);
	if (!PlayerState)
	{
		LL_ERR(this, "AVoiceChatManager::DeleteOdinIDForPlayer : PlayerState is nullptr");
		return;
	}

	// Get the MHAudioSubsystem
	if (UMHAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UMHAudioSubsystem>())
	{
		AudioSubsystem->PlayerOdinIDDestroyed(PlayerState->OdinID);
	}
}
