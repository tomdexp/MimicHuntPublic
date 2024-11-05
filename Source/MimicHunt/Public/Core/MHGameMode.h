#pragma once

#include "CoreMinimal.h"
#include "MHPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "UE5Coro.h"
#include "Audio/VoiceChat.h"
#include "MHGameMode.generated.h"

class AVoiceChatManager;
class AMHPlayerCharacter;
/**
 * This is the GameMode inherited by all the GameMode of the game
 * WARNING : IT EXIST ONLY THE SERVER
 */
UCLASS()
class MIMICHUNT_API AMHGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMHGameMode();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AVoiceChat> VoiceChatBlueprint;
	
	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;
	virtual void InitGameState() override;
	virtual void RespawnPlayerCharacter(AController* Controller);
	virtual void PlayerCharacterDied(AController* Controller);
	virtual bool AllowCheats(APlayerController* P) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
	UPROPERTY(Transient)
	TObjectPtr<class APersistentDataManager> PersistentDataManager;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMHPlayerCharacter> PlayerCharacterClass;

	// We use a WeakObjectPtr to ensure that the VoiceChatManager is not kept a reference to when it's destroyed
	UPROPERTY()
	TWeakObjectPtr<AVoiceChatManager> VoiceChatManager;

	UPROPERTY()
	bool bSpawnVoiceChatManagerOnInitGameState = true;
};
