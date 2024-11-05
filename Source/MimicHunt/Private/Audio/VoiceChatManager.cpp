#include "Audio/VoiceChatManager.h"

#include "Utils/LLog.h"

LL_FILE_CVAR(VoiceChatManager);

AVoiceChatManager::AVoiceChatManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AVoiceChatManager::BeginPlay()
{
	Super::BeginPlay();
	LL_DBG(this, "AVoiceChatManager::BeginPlay : Spawning voice chat manager, checking for voice chat actors to spawn");
}

void AVoiceChatManager::Destroyed()
{
	Super::Destroyed();
	LL_DBG(this, "AVoiceChatManager::Destroyed : Destroying voice chat manager");
	// Destroy all voice chat actors
	for (TWeakObjectPtr<AVoiceChat> VoiceChatActor : VoiceChatActors)
	{
		if (VoiceChatActor.IsValid())
		{
			LL_DBG(this, "AVoiceChatManager::Destroyed : Destroying voice chat actor with Odin ID {0}", VoiceChatActor->OdinID);
			VoiceChatActor->Destroy();
		}
	}
}

void AVoiceChatManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!HasAuthority()) return;
	// Print the number of controllers
	int32 ActiveCount = 0;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Iterator->Get();
		if (PC)
		{
			ActiveCount++;
		}
	}
	//LL_DBG(this, "AVoiceChatManager::Tick : There is {0} controllers", ActiveCount);
}
