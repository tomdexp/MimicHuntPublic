#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_GrantNearbyInteraction.generated.h"

/**
* This task sets a recurring timer to scan for interactable objects in a sphere around the player every X interval
* (configurable; default 0.1 seconds).

Each time it scans, it looks for actors that has a InteractableComponent.

Thus for an actor to be interactable, it must have an InteractableComponent.

The task constructs a list of all InteractableComponents found in the sphere around the player and grants the player
the abilities associated with them.
 */
UCLASS()
class MIMICHUNT_API UAbilityTask_GrantNearbyInteraction : public UAbilityTask
{
	GENERATED_BODY()
public:
	virtual void Activate() override;

	/** Wait until an overlap occurs. This will need to be better fleshed out so we can specify game specific collision requirements */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_GrantNearbyInteraction* GrantAbilitiesForNearbyInteractable(UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate);
private:

	virtual void OnDestroy(bool AbilityEnded) override;

	void QueryInteractables();

	float InteractionScanRange = 500;
	float InteractionScanRate = 0.100;

	FTimerHandle QueryTimerHandle;

	TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache;
};
