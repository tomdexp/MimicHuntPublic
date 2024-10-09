#include "Core/MHGameState.h"

#include "OnlineSubsystem.h"
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
		CurrentOnlineState = NewOnlineState;
		OnRep_CurrentOnlineState();
	}
}

void AMHGameState::OnRep_CurrentOnlineState()
{
	LL_DBG(this,"AMHGameState::OnRep_CurrentOnlineState : Current Online State: {0}", CurrentOnlineState);
}

void AMHGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMHGameState, CurrentOnlineState);
}
