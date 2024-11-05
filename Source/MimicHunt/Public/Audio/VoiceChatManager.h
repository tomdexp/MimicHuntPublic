#pragma once

#include "CoreMinimal.h"
#include "VoiceChat.h"
#include "GameFramework/Info.h"
#include "VoiceChatManager.generated.h"

class AMHPlayerController;

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
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void PostLogin(APlayerController* NewPlayer); // Called by the MHGameMode when a player logs in
	void Logout(AController* Exiting); // Called by the MHGameMode when a player logs out
	
	UPROPERTY()
	TSubclassOf<AVoiceChat> VoiceChatBlueprint;
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AVoiceChat>> SpawnedVoiceChatActors;
	
	void CreateVoiceChatForPlayer(FGuid OdinID);
	
	UE5Coro::TCoroutine<> CreateOdinIDForPlayerCoroutine(AMHPlayerController* PlayerController);
	
	void DeleteOdinIDForPlayer(AMHPlayerController* PlayerController);

	UPROPERTY()
	TArray<FGuid> OdinIDsInProcess; // To avoid create multiple Odin IDs for the same player
};
