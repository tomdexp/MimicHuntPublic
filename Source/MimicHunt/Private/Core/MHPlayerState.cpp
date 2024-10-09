#include "Core/MHPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(PlayerState);


void AMHPlayerState::NetMulticast_SetIsReadyInLobby_Implementation(bool bNewIsReadyInLobby)
{
	if (bIsReadyInLobby != bNewIsReadyInLobby)
	{
		LL_DBG(this, "AMHPlayerState::SetIsReadyInLobby : Player {0} ready in lobby = {1}", GetPlayerName(), bNewIsReadyInLobby);
		bIsReadyInLobby = bNewIsReadyInLobby;
		OnRep_IsReadyInLobby();
	}
}

bool AMHPlayerState::NetMulticast_SetIsReadyInLobby_Validate(bool bNewIsReadyInLobby)
{
	return true;
}

void AMHPlayerState::OnRep_IsReadyInLobby()
{
	LL_DBG(this, "AMHPlayerState::OnRep_IsReadyInLobby : Player {0} ready in lobby = {1}", GetPlayerName(), bIsReadyInLobby);
	LL_DBG(this, "AMHPlayerState::OnRep_IsReadyInLobby : Broadcast OnPlayerReadyInLobbyChanged with : {0}", bIsReadyInLobby);
	OnPlayerReadyInLobbyChanged.Broadcast(bIsReadyInLobby);
}

void AMHPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMHPlayerState, bIsReadyInLobby);
}
