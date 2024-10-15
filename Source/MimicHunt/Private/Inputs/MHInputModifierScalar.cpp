#include "Inputs/MHInputModifierScalar.h"

#include "EnhancedPlayerInput.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "Utils/MHPlayerSettingsSubsystem.h"

FInputActionValue UMHInputModifierScalar::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
                                                                   FInputActionValue CurrentValue, float DeltaTime)
{
	FVector NewValue = CurrentValue.Get<FVector>();
	
	// Guard against nullptrs
	if (!PlayerInput->GetWorld() || !PlayerInput->GetWorld()->GetGameInstance())
	{
		return Super::ModifyRaw_Implementation(PlayerInput, CurrentValue, DeltaTime);
	}
	
	// Get the player settings subsystem and bind the settings to the variables
	if (UMHPlayerSettingsSubsystem* PlayerSettingsSubsystem = PlayerInput->GetWorld()->GetGameInstance()->GetSubsystem<UMHPlayerSettingsSubsystem>())
	{
		bMouseLookInvertVertical = PlayerSettingsSubsystem->GetMouseInvertVertical();
		bMouseLookInvertHorizontal = PlayerSettingsSubsystem->GetMouseInvertHorizontal();
		MouseLookSensitivity = PlayerSettingsSubsystem->GetMouseSensitivity();
		bControllerLookInvertVertical = PlayerSettingsSubsystem->GetControllerInvertVertical();
		bControllerLookInvertHorizontal = PlayerSettingsSubsystem->GetControllerInvertHorizontal();
		ControllerLookSensitivity = PlayerSettingsSubsystem->GetControllerSensitivity();

		if (bIsController) // GAMEPAD
		{
			NewValue = FVector(NewValue.X * ControllerLookSensitivity, NewValue.Y * ControllerLookSensitivity, 0);
			if (bControllerLookInvertHorizontal)
			{
				NewValue.X *= -1;
			}
			if (bControllerLookInvertVertical)
			{
				NewValue.Y *= -1;
			}
		}
		else // MOUSE
		{
			NewValue = FVector(NewValue.X * MouseLookSensitivity, NewValue.Y * MouseLookSensitivity, 0);
			if (bMouseLookInvertHorizontal)
			{
				NewValue.X *= -1;
			}
			if (bMouseLookInvertVertical)
			{
				NewValue.Y *= -1;
			}
		}
	}
	
	return Super::ModifyRaw_Implementation(PlayerInput, NewValue, DeltaTime);
}
