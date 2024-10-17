#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MHGameplayAbility.generated.h"

UENUM(BlueprintType)
enum EMHAbilityActivationType
{
	Undefined,
	BindToInput,
	BindToTagEvent
};


/**
 * The base class for all gameplay abilities in the game.
 * We have two ways of binding abilities in this project:
 * - Bound to input
 * - Bound to a gameplay tag event
 */
UCLASS()
class MIMICHUNT_API UMHGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "GameplayAbility")
	EMHAbilityActivationType GetActivationType() const { return ActivationType; }

	EMHAbilityActivationType ActivationType = Undefined;
};
