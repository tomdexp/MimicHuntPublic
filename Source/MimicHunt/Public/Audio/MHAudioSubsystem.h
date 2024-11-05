#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MHAudioSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MHAudio, Log, All);

/**
 * UMHAudioSubsystem (UMimicHunterAudioSubsystem) is a class that is used to manage audio in the game.
 * It's accessible everywhere, anytime and it lives as long as the game instance lives.
 * It's not replicated on the network
 */
UCLASS()
class MIMICHUNT_API UMHAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void ExampleFunction();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StartRecordingWithRecognition(const FString& speechLanguage);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void GetGMEMessage(int& localUTCTime, int& messageType, int& code, FString& message1, FString& message2);

	UFUNCTION(BlueprintCallable, Category = "Audio|Odin")
	void StartupOdin();
	
	void PlayerOdinIDCreated(FGuid Guid);
	void PlayerOdinIDDestroyed(FGuid Guid);

	UPROPERTY()
	int32 VoiceRoomId = -1;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void CleanRoomId();

	UPROPERTY(BlueprintReadWrite, Category = "Audio|VoiceChat")
	bool bIsInSpectatorVoiceChat = false;

	UPROPERTY()
	TArray<FGuid> OdinIDs;

	UPROPERTY()
	int32 NumberOfConnectedPlayers = 0;
};
