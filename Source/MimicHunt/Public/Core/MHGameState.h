#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MHGameState.generated.h"

UENUM(BlueprintType)
enum EOnlineState
{
	Undefined,
	InLobby,
	InGame
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOnlineStateChanged, EOnlineState, NewOnlineState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerCountChanged, int32, NewPlayerCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnyPlayerReadyInLobbyChanged);

UCLASS()
class MIMICHUNT_API AMHGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AMHGameState();
	UFUNCTION(BlueprintCallable , Category = "Online")
	void SetOnlineState(EOnlineState NewOnlineState);
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentOnlineState, BlueprintReadOnly, Category = "Online")
	TEnumAsByte<EOnlineState> CurrentOnlineState;
	
	UPROPERTY(BlueprintAssignable, Category = "Online")
	FOnOnlineStateChanged OnOnlineStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Online")
	FOnPlayerCountChanged OnPlayerCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Online")
	FOnAnyPlayerReadyInLobbyChanged OnAnyPlayerReadyInLobbyChanged;

	UFUNCTION()
	void AnyPlayerReadyInLobbyChanged(bool bNewIsReadyInLobby);

	UFUNCTION(BlueprintCallable , Category = "Online")
	int32 GetReadyPlayersInLobbyCount();

	UFUNCTION(BlueprintCallable , Category = "Online")
	bool AreAllPlayersReadyInLobby();

	UFUNCTION()
	void OnRep_CurrentOnlineState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	UPROPERTY()
	int32 TestCounter;
};
