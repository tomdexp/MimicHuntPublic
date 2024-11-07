// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySystem/MHGameplayAbility.h"
#include "MHGameplayAbility_Interact.generated.h"

class UInteractableComponent;
/**
 * This class activate on the character when it spawns
* The ActivateAbility implementation for this Gameplay Ability:

- Start a UAbilityTask_GrantNearbyInteraction Gameplay Task (via the base C++ class, see below)
- Start a UAbilityTask_WaitForInteractableTargets_SingleLineTrace Gameplay Task
- Start async loop listening for interaction input key presses
 */
UCLASS(Abstract)
class MIMICHUNT_API UMHGameplayAbility_Interact : public UMHGameplayAbility
{
public:
	GENERATED_BODY()
	
	UMHGameplayAbility_Interact();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable)
	void UpdateInteractable(UInteractableComponent* Interactable);
	
	UFUNCTION(BlueprintCallable)
	void TriggerInteraction();
	
	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRange = 500;

	UPROPERTY()
	TWeakObjectPtr<UInteractableComponent> CurrentInteractable;

private:
	FGameplayTag AbilityInteractionActivateTag;
};
