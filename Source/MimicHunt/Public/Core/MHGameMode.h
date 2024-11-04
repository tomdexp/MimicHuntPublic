#pragma once

#include "CoreMinimal.h"
#include "MHPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "UE5Coro.h"
#include "Audio/VoiceChat.h"
#include "MHGameMode.generated.h"

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

	UE5Coro::TCoroutine<> SetupVoiceChatCoroutine(AMHPlayerController* PlayerController);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMHPlayerCharacter> PlayerCharacterClass;

	UPROPERTY()
	bool bSetupVoiceChatOnLogin = true;
};
