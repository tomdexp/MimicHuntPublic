#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MHPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerReadyInLobbyChanged, bool, bNewIsReadyInLobby);

UCLASS()
class MIMICHUNT_API AMHPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	UPROPERTY(ReplicatedUsing=OnRep_IsReadyInLobby, BlueprintReadOnly, Category = "Online")
	bool bIsReadyInLobby;

	// TODO : INVESTIGATE NETMULTICAST VS SERVER VS CLIENT RPC
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_SetIsReadyInLobby(bool bNewIsReadyInLobby);
	
	UFUNCTION()
	void OnRep_IsReadyInLobby();

	UPROPERTY(BlueprintAssignable, Category = "Online")
	FOnPlayerReadyInLobbyChanged OnPlayerReadyInLobbyChanged;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
