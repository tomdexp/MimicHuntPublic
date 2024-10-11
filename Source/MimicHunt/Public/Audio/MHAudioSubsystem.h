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
	void SetVoiceChatUserID(const int32 UserID);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetVoiceChatRoomID(const int32 RoomID);
};
