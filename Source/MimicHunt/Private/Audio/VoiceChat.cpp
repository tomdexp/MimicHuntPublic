#include "Audio/VoiceChat.h"

#include "Core/MHGameInstance.h"
#include "Core/MHPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(VoiceChat);

AVoiceChat::AVoiceChat()
{
	bReplicates = true;
	bAlwaysRelevant = true;
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
	
	if (HasAuthority())
	{
		OdinID = FGuid::NewGuid(); // This is replicated
		OnRep_OdinID();
	}
}

void AVoiceChat::OnRep_OdinID()
{
	LL_DBG(this, "AMHPlayerCharacter::OnRep_OdinID is {0}", OdinID);
	if (UMHGameInstance* GameInstance = GetGameInstance<UMHGameInstance>())
	{
		LL_DBG(this, "AMHPlayerCharacter::OnRep_OdinID Adding Odin ID to IdsToPlayerVoiceChatActors");
		GameInstance->IdsToPlayerVoiceChatActors.Add(OdinID, this);
	}
}

void AVoiceChat::OnRep_AssociatedPlayerState()
{
	// Set the name of this actor to BP_VoiceChat_{PlayerName}
	if (AssociatedPlayerState)
	{
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
	while (!AssociatedPlayerState)
	{
		co_await UE5Coro::Latent::NextTick();
	}
	co_return;
}

void AVoiceChat::OnReadyToInitOdin_BP_Implementation()
{
	// If the blueprint has an implementation, this will be ignored
}

