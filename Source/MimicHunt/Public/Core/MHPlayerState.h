#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "MHPlayerState.generated.h"

class UMHAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerReadyInLobbyChanged, bool, bNewIsReadyInLobby);

UCLASS()
class MIMICHUNT_API AMHPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AMHPlayerState();
	
	UPROPERTY(ReplicatedUsing=OnRep_IsReadyInLobby, BlueprintReadOnly, Category = "Online")
	bool bIsReadyInLobby;

	UPROPERTY()
	TObjectPtr<UMHAbilitySystemComponent> AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return static_cast<UAbilitySystemComponent*>(AbilitySystemComponent);
	}

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_SetIsReadyInLobby(bool bNewIsReadyInLobby);
	
	UFUNCTION()
	void OnRep_IsReadyInLobby();

	UPROPERTY(BlueprintAssignable, Category = "Online")
	FOnPlayerReadyInLobbyChanged OnPlayerReadyInLobbyChanged;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
