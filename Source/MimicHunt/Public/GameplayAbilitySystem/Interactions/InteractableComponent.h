#pragma once

#include "CoreMinimal.h"
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

	// Granted when the player detects the interactable object.
	// Activated when the player interacts with the object.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> InteractionAbility;
};
