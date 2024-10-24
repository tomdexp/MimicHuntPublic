#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MHAbilitySystemComponent.generated.h"


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
};
