#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MHGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EMHAbilityActivationType : uint8
{
	Undefined,
	BindToInput,
	BindToTagEvent
};

UENUM(BlueprintType)
enum class EMHAbilityActivationInputType : uint8
{
	Undefined,
	JumpAction,
	SprintActionStart,
	SprintActionEnd,
	InteractActionStart,
	InteractActionEnd
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
	UMHGameplayAbility();
	
	UFUNCTION(BlueprintCallable, Category = "GameplayAbility")
	EMHAbilityActivationType GetActivationType() const { return ActivationType; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayAbility")
	EMHAbilityActivationType ActivationType = EMHAbilityActivationType::Undefined;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayAbility", meta = (EditCondition = "ActivationType == EMHAbilityActivationType::BindToTagEvent", EditConditionHides = true))
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayAbility", meta = (EditCondition = "ActivationType == EMHAbilityActivationType::BindToInput", EditConditionHides = true))
	EMHAbilityActivationInputType ActivationInputType = EMHAbilityActivationInputType::Undefined;
};
