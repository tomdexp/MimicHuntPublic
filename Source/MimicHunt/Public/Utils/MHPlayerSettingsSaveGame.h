#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MHPlayerSettingsSaveGame.generated.h"

/**
 * This is where we will save the player settings (for graphics, controls, etc)
 * It's similar to Unity's PlayerPrefs, however the communication with this object is made with the MHPlayerSettingsSubsystem
 */
UCLASS()
class MIMICHUNT_API UMHPlayerSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bMouseInvertVertical = false;

	UPROPERTY()
	bool bMouseInvertHorizontal = false;

	UPROPERTY()
	float MouseSensitivity = 1.0f;

	UPROPERTY()
	bool bControllerInvertVertical = false;

	UPROPERTY()
	bool bControllerInvertHorizontal = false;

	UPROPERTY()
	float ControllerSensitivity = 1.0f;
};
