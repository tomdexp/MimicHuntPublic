#pragma once

#include "CoreMinimal.h"
#include "VoiceChat.h"
#include "GameFramework/Info.h"
#include "VoiceChatManager.generated.h"

/**
 * This class is spawned by the game mode and is responsible for managing voice chat actors.
 * This actor is not persistent between server travels. It should destroy all voice chat actors when it's destroyed.
 */
UCLASS()
class MIMICHUNT_API AVoiceChatManager : public AInfo
{
	GENERATED_BODY()

public:
	AVoiceChatManager();
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void Tick(float DeltaSeconds) override;

	
	TArray<TWeakObjectPtr<AVoiceChat>> VoiceChatActors;
};
