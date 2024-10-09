#include "Core/MHGameState.h"

#include "OnlineSubsystem.h"
#include "Core/MHPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(GameState);

AMHGameState::AMHGameState()
{
	CurrentOnlineState = Undefined;
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
}

void AMHGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	LL_DBG(this,"AMHGameState::AddPlayerState : There is now {0} players", PlayerArray.Num());
	OnPlayerCountChanged.Broadcast(PlayerArray.Num());

	if (AMHPlayerState* MHPlayerState = Cast<AMHPlayerState>(PlayerState))
	{
		MHPlayerState->OnPlayerReadyInLobbyChanged.AddDynamic(this, &AMHGameState::AnyPlayerReadyInLobbyChanged);
	}
}

void AMHGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	LL_DBG(this,"AMHGameState::RemovePlayerState : There is now {0} players", PlayerArray.Num());
	OnPlayerCountChanged.Broadcast(PlayerArray.Num());
	if (AMHPlayerState* MHPlayerState = Cast<AMHPlayerState>(PlayerState))
	{
		MHPlayerState->OnPlayerReadyInLobbyChanged.RemoveDynamic(this, &AMHGameState::AnyPlayerReadyInLobbyChanged);
	}
}

void AMHGameState::AnyPlayerReadyInLobbyChanged(bool bNewIsReadyInLobby)
{
	OnAnyPlayerReadyInLobbyChanged.Broadcast();
}

int32 AMHGameState::GetReadyPlayersInLobbyCount()
{
	int32 ReadyPlayersCount = 0;
	for (int32 i = 0; i < PlayerArray.Num(); i++)
	{
		if (AMHPlayerState* MHPlayerState = Cast<AMHPlayerState>(PlayerArray[i]))
		{
			if (MHPlayerState->bIsReadyInLobby)
			{
				ReadyPlayersCount++;
			}
		}
	}
	return ReadyPlayersCount;
}

bool AMHGameState::AreAllPlayersReadyInLobby()
{
	return GetReadyPlayersInLobbyCount() == PlayerArray.Num();
}
