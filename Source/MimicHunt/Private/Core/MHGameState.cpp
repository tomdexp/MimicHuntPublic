#include "Core/MHGameState.h"

#include "OnlineSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(GameState);

AMHGameState::AMHGameState()
{
	CurrentOnlineState = Undefined;
	PrimaryActorTick.bCanEverTick = true;
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
	LL_DBG("AMHGameState::OnRep_CurrentOnlineState : Current Online State: {0}", CurrentOnlineState);
	//UE_LOG(LogTemp, Log, TEXT("AMHGameState::OnRep_CurrentOnlineState : Current Online State: %s"), *UEnum::GetValueAsString(CurrentOnlineState));
}

void AMHGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMHGameState, CurrentOnlineState);
}

void AMHGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	LL_DBG("AMHGameState::Tick : Current Online State: {0}", CurrentOnlineState);
}
