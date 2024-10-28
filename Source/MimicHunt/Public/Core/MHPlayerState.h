#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "MHPlayerState.generated.h"

struct FOnAttributeChangeData;
class UMHAttributeSetPlayer;
class UMHAbilitySystemComponent;

UENUM(BlueprintType)
enum class EMHPlayerLifeType : uint8
{
	Undefined,
	Alive,
	Injured,
	Dead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerReadyInLobbyChanged, bool, bNewIsReadyInLobby);

UCLASS()
class MIMICHUNT_API AMHPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AMHPlayerState();
	
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentPlayerLifeType, BlueprintReadOnly, Category = "Player")
	EMHPlayerLifeType CurrentPlayerLifeType = EMHPlayerLifeType::Undefined;

	UFUNCTION()
	void OnRep_CurrentPlayerLifeType();

	UPROPERTY()
	TObjectPtr<UMHAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UMHAttributeSetPlayer> AttributeSetPlayer;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UMHAttributeSetPlayer* GetAttributeSetPlayer() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FGameplayTag DeadTag;
	
	float GetHealth() const;
	float GetMaxHealth() const;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);

	UFUNCTION(BlueprintCallable, Category = "Player")
	bool IsAlive() const;

};
