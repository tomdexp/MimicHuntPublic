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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SETTINGS")
	bool bAdvancedEditMode = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT", meta = (EditCondition = "bAdvancedEditMode", EditConditionHides = true))
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT", meta = (EditCondition = "bAdvancedEditMode", EditConditionHides = true))
	TObjectPtr<UInputAction> InputActionMove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT", meta = (EditCondition = "bAdvancedEditMode", EditConditionHides = true))
	TObjectPtr<UInputAction> InputActionLook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT", meta = (EditCondition = "bAdvancedEditMode", EditConditionHides = true))
	TObjectPtr<UInputAction> InputActionJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT", meta = (EditCondition = "bAdvancedEditMode", EditConditionHides = true))
	TObjectPtr<UInputAction> InputActionCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT", meta = (EditCondition = "bAdvancedEditMode", EditConditionHides = true))
	TObjectPtr<UInputAction> InputActionCrouchToggle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT", meta = (EditCondition = "bAdvancedEditMode", EditConditionHides = true))
	TObjectPtr<UInputAction> InputActionSprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT", meta = (EditCondition = "bAdvancedEditMode", EditConditionHides = true))
	TObjectPtr<UInputAction> InputActionSprintToggle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT", meta = (EditCondition = "bAdvancedEditMode", EditConditionHides = true))
	TObjectPtr<UInputAction> InputActionPrimaryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT", meta = (EditCondition = "bAdvancedEditMode", EditConditionHides = true))
	TObjectPtr<UInputAction> InputActionSecondaryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "INPUT", meta = (EditCondition = "bAdvancedEditMode", EditConditionHides = true))
	TObjectPtr<UInputAction> InputActionInteract;


	/*In cm/s*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MOVEMENT")
	float WalkSpeed = 200.0f;

	/*In cm/s*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MOVEMENT")
	float SprintSpeed = 400.0f;

	/*In cm/s*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MOVEMENT")
	float CrouchSpeed = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MOVEMENT")
	float JumpVelocity = 300.0f;
};
