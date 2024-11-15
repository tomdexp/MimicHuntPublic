#include "Audio/VoiceChat.h"

#include "EngineUtils.h"
#include "Audio/MHAudioSubsystem.h"
#include "Core/MHGameInstance.h"
#include "Core/MHPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(VoiceChat);

AVoiceChat::AVoiceChat()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
	AkOdinInputComponent = CreateDefaultSubobject<UAkOdinInputComponent>(TEXT("AkOdinInputComponent"));
	AkOdinInputComponentSpectator = CreateDefaultSubobject<UAkOdinInputComponent>(TEXT("AkOdinInputComponentSpectator"));
}

void AVoiceChat::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AVoiceChat, OdinID)
	DOREPLIFETIME(AVoiceChat, AssociatedPlayerState)
}

void AVoiceChat::BeginPlay()
{
	Super::BeginPlay();
}

void AVoiceChat::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!HasAuthority()) return;
	// If the AssociatedPlayerState is not valid and we have an Odin ID, try to find the player state
	if (!AssociatedPlayerState.IsValid() && OdinID != FGuid())
	{
		LL_DBG(this, "AVoiceChat::Tick : AssociatedPlayerState is not valid, trying to find the player state with Odin ID {0}", OdinID);
		// Try to find the player state with the Odin ID
		// Iterate through all PlayerStates Actors in the world
		for (TActorIterator<AMHPlayerState> It(GetWorld()); It; ++It)
		{
			AMHPlayerState* PlayerState = *It;
			if (PlayerState && PlayerState->OdinID == OdinID)
			{
				LL_DBG(this, "AVoiceChat::Tick : Found the player state with Odin ID {0} : {1}", OdinID, *PlayerState->GetPlayerName());
				AssociatedPlayerState = PlayerState;
				OnRep_AssociatedPlayerState();
			}
		}
	}
}

void AVoiceChat::Destroyed()
{
	Super::Destroyed();
	LL_DBG(this, "AVoiceChat::Destroyed : Destroying voice chat actor with Odin ID {0}", OdinID);
	if (UMHGameInstance* GameInstance = GetGameInstance<UMHGameInstance>())
	{
		LL_DBG(this, "AVoiceChat::Destroyed : Removing Odin ID ({0}) from IdsToPlayerVoiceChatActors", OdinID);
		GameInstance->IdsToPlayerVoiceChatActors.Remove(OdinID);
	}
}

void AVoiceChat::OnRep_OdinID()
{
	LL_DBG(this, "AVoiceChat::OnRep_OdinID is {0}", OdinID);
	if (UMHGameInstance* GameInstance = GetGameInstance<UMHGameInstance>())
	{
		LL_DBG(this, "AVoiceChat::OnRep_OdinID Adding Odin ID ({0}) to IdsToPlayerVoiceChatActors", OdinID);
		GameInstance->IdsToPlayerVoiceChatActors.Add(OdinID, this);
	}
}

void AVoiceChat::OnRep_AssociatedPlayerState()
{
	LL_DBG(this, "AVoiceChat::OnRep_AssociatedPlayerState is {0}", *AssociatedPlayerState->GetPlayerName());
	if (AssociatedPlayerState.IsValid())
	{
		// Set the name of this actor to BP_VoiceChat_{PlayerName}
		SetActorLabel(FString::Printf(TEXT("BP_VoiceChat_%s"), *AssociatedPlayerState->GetPlayerName()));
	}
}

FVoidCoroutine AVoiceChat::WaitForOdinID(FLatentActionInfo LatentInfo)
{
	while (OdinID == FGuid())
	{
		co_await UE5Coro::Latent::NextTick();
	}
	co_return;
}

FVoidCoroutine AVoiceChat::WaitForAssociatedPlayerState(FLatentActionInfo LatentInfo)
{
	while (!AssociatedPlayerState.IsValid())
	{
		co_await UE5Coro::Latent::NextTick();
	}
	co_return;
}

FVoidCoroutine AVoiceChat::WaitForPawn(FLatentActionInfo LatentInfo)
{
	while (!AssociatedPlayerState.IsValid() || !AssociatedPlayerState->GetPawn())
	{
		co_await UE5Coro::Latent::NextTick();
	}
	co_return;
}


void AVoiceChat::OnReadyToInitOdin_BP_Implementation()
{
	// If the blueprint has an implementation, this will be ignored
}

