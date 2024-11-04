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
}

