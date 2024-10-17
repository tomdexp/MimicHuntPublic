#include "Core/MHPlayerState.h"

#include "GameplayAbilitySystem/MHAbilitySystemComponent.h"
#include "GameplayAbilitySystem/AttributeSets/MHAttributeSetPlayer.h"
#include "Net/UnrealNetwork.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(PlayerState);


AMHPlayerState::AMHPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSetPlayer = CreateDefaultSubobject<UMHAttributeSetPlayer>(TEXT("AttributeSetPlayer"));

	NetUpdateFrequency = 50.0f;
}

void AMHPlayerState::Server_SetIsReadyInLobby_Implementation(bool bNewIsReadyInLobby)
{
	if (bIsReadyInLobby != bNewIsReadyInLobby)
	{
		//LL_DBG(this, "AMHPlayerState::SetIsReadyInLobby : Player {0} ready in lobby = {1}", GetPlayerName(), bNewIsReadyInLobby);
		bIsReadyInLobby = bNewIsReadyInLobby;
		OnRep_IsReadyInLobby();
	}
}

bool AMHPlayerState::Server_SetIsReadyInLobby_Validate(bool bNewIsReadyInLobby)
{
	return true;
}

UAbilitySystemComponent* AMHPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UMHAttributeSetPlayer* AMHPlayerState::GetAttributeSetPlayer() const
{
	return AttributeSetPlayer;
}

void AMHPlayerState::OnRep_IsReadyInLobby()
{
	LL_DBG(this, "AMHPlayerState::OnRep_IsReadyInLobby : Player {0} ready in lobby = {1}", GetPlayerName(), bIsReadyInLobby);
	OnPlayerReadyInLobbyChanged.Broadcast(bIsReadyInLobby);
}

void AMHPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMHPlayerState, bIsReadyInLobby);
}
