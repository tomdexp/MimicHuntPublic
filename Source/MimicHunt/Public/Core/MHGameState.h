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

	UFUNCTION()
	void OnRep_CurrentOnlineState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
