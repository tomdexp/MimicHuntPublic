#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "MHPlayerController.generated.h"

class UMHPlayerData;

/**
 * This class handles all inputs to the character
 * It should only request actions to the character and then the character will decide what to do
 * This will allow us to have the MHPlayerCharacter be possessed by an AIController and still work
 */
UCLASS()
class MIMICHUNT_API AMHPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "DATA")
	TObjectPtr<UMHPlayerData> PlayerData;
	
	void RequestLookAction(const FInputActionValue& InputActionValue);
	void RequestMoveAction(const FInputActionValue& InputActionValue);
	void RequestJumpAction(const FInputActionValue& InputActionValue);
	void RequestSprintActionStart(const FInputActionValue& InputActionValue);
	void RequestSprintActionEnd(const FInputActionValue& InputActionValue);
	void RequestSprintToggleAction(const FInputActionValue& InputActionValue);
	void RequestCrouchActionStart(const FInputActionValue& InputActionValue);
	void RequestCrouchActionEnd(const FInputActionValue& InputActionValue);
	void RequestCrouchToggleAction(const FInputActionValue& InputActionValue);
	void RequestPrimaryAction(const FInputActionValue& InputActionValue);
	void RequestSecondaryAction(const FInputActionValue& InputActionValue);
};
