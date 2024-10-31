#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AkOdinInputComponent.h"
#include "UE5Coro/UnrealTypes.h"
#include "VoiceChat.generated.h"


class AMHPlayerState;

UCLASS()
class MIMICHUNT_API AVoiceChat : public AActor
{
	GENERATED_BODY()
	
public:	
	AVoiceChat();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, ReplicatedUsing = OnRep_OdinID)
	FGuid OdinID;

	UFUNCTION()
	void OnRep_OdinID();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "VoiceChat", Replicated)
	TObjectPtr<AMHPlayerState> AssociatedPlayerState;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "VoiceChat")
	TObjectPtr<UAkOdinInputComponent> AkOdinInputComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "VoiceChat")
	TObjectPtr<UAkOdinInputComponent> AkOdinInputComponentSpectator;
	
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
	FVoidCoroutine WaitForOdinID(FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
	FVoidCoroutine WaitForAssociatedPlayerState(FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintNativeEvent)
	void OnReadyToInitOdin_BP();
};
