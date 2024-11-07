#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MHAbilitySystemComponent.generated.h"

enum class EMHAbilityActivationInputType : uint8;
// Represent a gameplay ability which is bound to two gameplay tag event (start and end)
USTRUCT()
struct FTagBoundGameplayAbility
{
	GENERATED_BODY()
	FGameplayAbilitySpecHandle AbilityHandle;
	FGameplayTag GameplayTagStart;
	FGameplayTag GameplayTagCancel;
};


UCLASS(ClassGroup=(MimicHunt), meta=(BlueprintSpawnableComponent))
class MIMICHUNT_API UMHAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UMHAbilitySystemComponent();

	UPROPERTY()
	bool bCharacterAbilitiesGiven = false;

	UPROPERTY()
	bool bStartupEffectsApplied = false;
	
	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UPROPERTY()
	TArray<FTagBoundGameplayAbility> TagBoundAbilities;
};
