#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "MHPlayerState.generated.h"

class UMHAttributeSetPlayer;
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

	UPROPERTY()
	TObjectPtr<UMHAttributeSetPlayer> AttributeSetPlayer;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UMHAttributeSetPlayer* GetAttributeSetPlayer() const;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_SetIsReadyInLobby(bool bNewIsReadyInLobby);
	
	UFUNCTION()
	void OnRep_IsReadyInLobby();

	UPROPERTY(BlueprintAssignable, Category = "Online")
	FOnPlayerReadyInLobbyChanged OnPlayerReadyInLobbyChanged;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
