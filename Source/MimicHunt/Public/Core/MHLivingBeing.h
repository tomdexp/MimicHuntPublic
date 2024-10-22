#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySystem/AttributeSets/MHAttributeSetLivingBeing.h"
#include "MHLivingBeing.generated.h"

class UMHAbilitySystemComponent;
/**
 * This is the base class for all the living beings of the game
 * It has two C++ children :
 * - AMHPlayerCharacter : The player character
 * - AMHMimicCharacter : The mimic character
 *
 * Its where we put methods and attributes that are common to all the living beings of the game
 * Like utilities methods for animations (GetSpeed(), GetDirection(), etc)
 */
UCLASS()
class MIMICHUNT_API AMHLivingBeing : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMHLivingBeing();
	
	UFUNCTION(BlueprintCallable, Category = "LivingBeing")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "LivingBeing")
	float GetHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "LivingBeing")
	void SetHealth(float Health);

	UFUNCTION(BlueprintCallable, Category = "LivingBeing")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "LivingBeing")
	float GetSpeed() const;

	/**
	 * \brief Returns the current pitch rotation clamped between -90 (looking down) and 90 (looking at the sky) degrees.
	 */
	UFUNCTION(BlueprintCallable, Category = "LivingBeing")
	float GetViewAngleVertical() const;

	UFUNCTION(BlueprintCallable, Category = "LivingBeing")
	float GetMovementDirectionX() const;

	UFUNCTION(BlueprintCallable, Category = "LivingBeing")
	float GetMovementDirectionY() const;

	UFUNCTION(BlueprintCallable, Category = "LivingBeing")
	float GetMovementDirectionXNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "LivingBeing")
	float GetMovementDirectionYNormalized() const;
	
	/************************************************************************/
	/* 						GAMEPLAY ABILITY SYSTEM			                */
	/************************************************************************/
	UPROPERTY()
	TObjectPtr<UMHAbilitySystemComponent> AbilitySystemComponent;

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY()
	TObjectPtr<UMHAttributeSetLivingBeing> AttributeSetLivingBeing;

	// Default abilities for this Character. These will be removed on Character death and regiven if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UMHGameplayAbility>> CharacterAbilities;

	// Default attributes for a character for initializing on spawn/respawn.
	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	// These effects are only applied one time on startup (like a health regen)
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();

	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

	FGameplayTag DeadTag;
};
