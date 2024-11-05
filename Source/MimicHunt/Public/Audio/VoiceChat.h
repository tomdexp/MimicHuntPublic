#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AkOdinInputComponent.h"
#include "UE5Coro/UnrealTypes.h"
#include "VoiceChat.generated.h"


class AMHPlayerState;

/*
 * VoiceChat is the class that represents a player's voice chat. It is responsible for managing the player's voice chat input and output.
 * This actor is replicated to all clients and the server.
 * So every instance of the game has as many voice chat actors as there are players.
 * It's persistent between server travels with MHGameMode::GetSeamlessTravelActorList()
 */
UCLASS()
class MIMICHUNT_API AVoiceChat : public AActor
{
	GENERATED_BODY()
	
public:	
	AVoiceChat();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Destroyed() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing = OnRep_OdinID)
	FGuid OdinID;

	UFUNCTION()
	void OnRep_OdinID();

	// We use a Weak pointer to avoid preventing the deletion of the player state between seamless travels.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "VoiceChat", ReplicatedUsing=OnRep_AssociatedPlayerState)
	TWeakObjectPtr<AMHPlayerState> AssociatedPlayerState;

	UFUNCTION()
	void OnRep_AssociatedPlayerState();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "VoiceChat")
	TObjectPtr<UAkOdinInputComponent> AkOdinInputComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "VoiceChat")
	TObjectPtr<UAkOdinInputComponent> AkOdinInputComponentSpectator;
	
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
	FVoidCoroutine WaitForOdinID(FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
	FVoidCoroutine WaitForAssociatedPlayerState(FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
	FVoidCoroutine WaitForPawn(FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintNativeEvent)
	void OnReadyToInitOdin_BP();
};
