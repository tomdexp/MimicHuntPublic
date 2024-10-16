#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MHPlayerData.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 * This is static data for the player (like a Unity ScriptableObject)
 */
UCLASS()
class MIMICHUNT_API UMHPlayerData : public UDataAsset
{
	GENERATED_BODY()
public:
	/** INPUTS */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT")
	TObjectPtr<UInputAction> InputActionMove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT")
	TObjectPtr<UInputAction> InputActionLook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT")
	TObjectPtr<UInputAction> InputActionJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT")
	TObjectPtr<UInputAction> InputActionCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT")
	TObjectPtr<UInputAction> InputActionCrouchToggle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT")
	TObjectPtr<UInputAction> InputActionSprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT")
	TObjectPtr<UInputAction> InputActionSprintToggle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT")
	TObjectPtr<UInputAction> InputActionPrimaryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT")
	TObjectPtr<UInputAction> InputActionSecondaryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT")
	TObjectPtr<UInputAction> InputActionInteract;

	/** MOVEMENT */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MOVEMENT")
	float WalkSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MOVEMENT")
	float SprintSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MOVEMENT")
	float CrouchSpeed = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MOVEMENT")
	float JumpVelocity = 300.0f;
};
