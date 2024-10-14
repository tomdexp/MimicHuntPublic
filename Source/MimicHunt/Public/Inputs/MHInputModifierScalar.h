#pragma once

#include "CoreMinimal.h"
#include "InputModifiers.h"
#include "MHInputModifierScalar.generated.h"

/**
 * This class is a scalar input modifier that use the PlayerSettings Subsystem to modify the input value.
 * It was made for the look actions, so it has a different sensitivity and invert settings for the mouse and the controller.
 */
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "MH Look Settings"))
class UMHInputModifierScalar : public UInputModifier
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bIsController;
protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
private:
	bool bMouseLookInvertVertical;
	bool bMouseLookInvertHorizontal;
	float MouseLookSensitivity;
	
	bool bControllerLookInvertVertical;
	bool bControllerLookInvertHorizontal;
	float ControllerLookSensitivity;
};
