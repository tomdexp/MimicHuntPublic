#include "Core/MHGameState.h"

#include "OnlineSubsystem.h"
#include "Audio/MHAudioSubsystem.h"
#include "Audio/VoiceChat.h"
#include "Core/MHGameInstance.h"
#include "Core/MHPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Utils/LLog.h"

class UMHAudioSubsystem;
LL_FILE_CVAR(MHBaseGameState);

AMHGameState::AMHGameState()
{
	CurrentOnlineState = Undefined;
	VoiceRoomId = -1;
	PrimaryActorTick.bCanEverTick = true;
}

void AMHGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) // Only the server should generate the room id
	{
		if (UWorld* World = GetWorld()) // Get the MHAudioSubsystem via the World
		{
			if (UMHAudioSubsystem* AudioSubsystem = World->GetGameInstance()->GetSubsystem<UMHAudioSubsystem>())
			{
				if (AudioSubsystem->VoiceRoomId == -1) // There was no room id set (first time)
				{
					// Create a random 9 digits int
					VoiceRoomId = FMath::RandRange(100000000, 999999999);
					AudioSubsystem->VoiceRoomId = VoiceRoomId;
					LL_DBG(this,"AMHGameState::BeginPlay : VoiceRoomId was -1, set to {0}", VoiceRoomId);
				}
			}
		}
	}
}

void AMHGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!HasAuthority()) return;
	
	// Print the number of Controller with GetWorld()->GetPlayerControllerIterator()
	// int32 ActiveCount = 0;
	//
	// for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	// {
	// 	APlayerController* PC = Iterator->Get();
	// 	if (PC)
	// 	{
	// 		ActiveCount++;
	// 	}
	// }
	//LL_DBG(this, "AMHGameState::Tick : There is {0} controllers", ActiveCount);
	// Print the map name
	// LL_DBG(this,"AMHGameState::Tick : Map name is {0}", GetWorld()->GetMapName());
}

void AMHGameState::SetOnlineState(EOnlineState NewOnlineState)
{
	if (HasAuthority())
	{
		if (CurrentOnlineState != NewOnlineState)
		{
			CurrentOnlineState = NewOnlineState;
			OnRep_CurrentOnlineState();
		}
	}
}


void AMHGameState::OnRep_CurrentOnlineState()
{
	LL_DBG(this,"AMHGameState::OnRep_CurrentOnlineState : Current Online State: {0}", CurrentOnlineState);
	LL_DBG(this,"AMHGameState::OnRep_CurrentOnlineState : Broadcast OnOnlineStateChanged with : {0}", CurrentOnlineState);
	OnOnlineStateChanged.Broadcast(CurrentOnlineState);
}

void AMHGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMHGameState, CurrentOnlineState);
	DOREPLIFETIME(AMHGameState, VoiceRoomId);
}

void AMHGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	LL_DBG(this,"AMHGameState::AddPlayerState : There is now {0} players", PlayerArray.Num());
	OnPlayerCountChanged.Broadcast(PlayerArray.Num());

	if (HasAuthority())
	{
		//SetupVoiceChatForPlayerStateCoroutine(PlayerState);
	}
}

void AMHGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	LL_DBG(this, "AMHGameState::RemovePlayerState : There is now {0} players", PlayerArray.Num());
	OnPlayerCountChanged.Broadcast(PlayerArray.Num());

	// TODO : De-spawning voice actors shouldn't be done here, since RemovePlayerState is called when traveling between levels
	//		  It should be done when the player decide to really leave the game by disconnecting or quitting

	
	// Despawn the voice chat actor associated with this player
	/*if (HasAuthority())
	{
		// If the voice chat actor exists for the player, we destroy it
		if (UMHGameInstance* GameInstance = GetGameInstance<UMHGameInstance>())
		{
			// Does this player state have a voice chat actor ?
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClass(GameInstance->GetWorld(), AVoiceChat::StaticClass(), FoundActors);

			bool AssociatedVoiceChatFound = false;
			
			for (AActor* Actor : FoundActors)
			{
				if (Actor)
				{
					if (AVoiceChat* VoiceChat = Cast<AVoiceChat>(Actor))
					{
						if (VoiceChat->AssociatedPlayerState == PlayerState)
						{
							// There is already a voice chat actor associated with this player
							LL_DBG(this,"AMHGameState::RemovePlayerState : Found an associated voice chat actor for this player, destroying it");
							VoiceChat->Destroy();
							AssociatedVoiceChatFound = true;
						}
					}
				}
			}

			if (!AssociatedVoiceChatFound)
			{
				LL_DBG(this,"AMHGameState::RemovePlayerState : No voice chat actor found for this player");
			}
		}
	}*/
}

UE5Coro::TCoroutine<> AMHGameState::SetupVoiceChatForPlayerStateCoroutine(APlayerState* PlayerState)
{
	if (UMHGameInstance* GameInstance = GetGameInstance<UMHGameInstance>())
	{
		// Are we in a Transition Scene ?
		if (GameInstance->IsTransitionScene())
		{
			LL_DBG(this, "AMHGameState::AddPlayerState : Ignoring voice chat actor creation because we are in a transition scene"); 
			co_return;
		}
		
		// Does this player state have a voice chat actor ?
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GameInstance->GetWorld(), AVoiceChat::StaticClass(), FoundActors);

		bool AssociatedVoiceChatFound = false;

		for (AActor* Actor : FoundActors)
		{
			if (Actor)
			{
				if (AVoiceChat* VoiceChat = Cast<AVoiceChat>(Actor))
				{
					if (VoiceChat->OdinID == Cast<AMHPlayerState>(PlayerState)->OdinID)
					{
						// There is already a voice chat actor associated with this player
						LL_DBG(this, "AMHGameState::AddPlayerState : Found an associated voice chat actor with OdinID ({0}) for the player ({1}), will not spawn a new one", VoiceChat->OdinID, PlayerState->GetPlayerName());
						AssociatedVoiceChatFound = true;
					}
				}
			}
		}

		// This player doesn't have a voice chat associated, so we spawn
		if (!AssociatedVoiceChatFound)
		{
			LL_DBG(this, "AMHGameState::AddPlayerState : No voice chat actor found for the player (id:{0}), creating a new one...", PlayerState->GetPlayerId());
			// Set up spawn parameters (optional)
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// Set the spawn location and rotation
			FVector SpawnLocation = FVector::ZeroVector;
			FRotator SpawnRotation = FRotator::ZeroRotator;

			// Spawn the actor
			AVoiceChat* NewVoiceChat = GetWorld()->SpawnActor<AVoiceChat>(
				VoiceChatBlueprint, SpawnLocation, SpawnRotation, SpawnParams);
			AMHPlayerState* PlayerStateToAssociate = Cast<AMHPlayerState>(PlayerState);
			NewVoiceChat->AssociatedPlayerState = PlayerStateToAssociate;
			//PlayerStateToAssociate->OdinID = NewVoiceChat->OdinID;

			// Give ownership of the actor the player
			NewVoiceChat->SetOwner(PlayerState->GetPlayerController());
			LL_DBG(
				this,
				"AMHGameState::AddPlayerState : Created a new voice chat actor, because player state (id:{0}) is new",
				PlayerState->GetPlayerId());
		}
	}
}


