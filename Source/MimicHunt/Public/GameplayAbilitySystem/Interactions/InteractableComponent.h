#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"

/**
 * This component is used to mark an actor as interactable.
 * An interactable component has a gameplay ability that can be triggered when the player interacts with it.
 * It is granted when the player detects the interactable object.
 * It is activated when the player interacts with the object.
 */

class UGameplayAbility;

UCLASS(meta=(BlueprintSpawnableComponent))
class MIMICHUNT_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText InteractionText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsInteractable = true;

	/**
	 * There is 2 ways of activating the interaction ability
	 * - 1 : The player that interacts with the object is granted an ability that is triggered when the player interacts with the object.
	 * - 2 : The object we're interacting with has its own ability system and interaction ability, that we can activate instead.
	 */
	
	// Granted when the player detects the interactable object.
	// Activated when the player interacts with the object.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> InteractionAbility;

	// If we want to trigger the ability on a different target than the player, we can set this for option 2.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TWeakObjectPtr<class UAbilitySystemComponent> TargetAbilitySystem = nullptr;

	// The ability spec to activate on the object if we use option 2.
	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilitySpecHandle TargetInteractionAbilityHandle;
};
