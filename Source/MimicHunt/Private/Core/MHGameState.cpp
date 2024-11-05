#include "Core/MHGameState.h"

#include "OnlineSubsystem.h"
#include "Audio/MHAudioSubsystem.h"
#include "Audio/VoiceChat.h"
#include "Audio/VoiceChatManager.h"
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
	PrimaryActorTick.bCanEverTick = true;
}

void AMHGameState::BeginPlay()
{
	Super::BeginPlay();
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
	DOREPLIFETIME(AMHGameState, VoiceRoomOdinID);
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

void AMHGameState::OnRep_VoiceRoomOdinID()
{
	LL_DBG(this, "AMHGameState::OnRep_VoiceRoomOdinID : VoiceRoomOdinID is {0}", VoiceRoomOdinID);
}

FVoidCoroutine AMHGameState::WaitForVoiceRoomOdinID(FLatentActionInfo LatentInfo)
{
	LL_DBG(this, "AMHGameState::WaitForVoiceRoomOdinID : Waiting for VoiceRoomOdinID to be set");
	while (VoiceRoomOdinID == FGuid())
	{
		
		co_await UE5Coro::Latent::NextTick();
	}
	co_return;
}

