#pragma once

#include "CoreMinimal.h"
#include "MHPlayerSettingsSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MHPlayerSettingsSubsystem.generated.h"

class UCheckBox;
class UMHPlayerSettingsSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerSettingsChanged);

/**
 * This is the Subsystem class for the Player Settings.
 * It is used to store the player settings and to save them to the disk by using the MHPlayerSettingsSaveGame class.
 * We assume automatic save when settings are changed. So there is an implicit save when the settings are changed.
 */
UCLASS()
class MIMICHUNT_API UMHPlayerSettingsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintAssignable, Category = "Player Settings")
	FPlayerSettingsChanged OnPlayerSettingsChanged;

	// MOUSE INVERT VERTICAL
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	bool GetMouseInvertVertical() const;
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	void SetMouseInvertVertical(bool bInvertVertical);

	// MOUSE INVERT HORIZONTAL
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	bool GetMouseInvertHorizontal() const;
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	void SetMouseInvertHorizontal(bool bInvertHorizontal);

	// MOUSE SENSITIVITY
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	float GetMouseSensitivity() const;
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	void SetMouseSensitivity(float Sensitivity);

	// CONTROLLER INVERT VERTICAL
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	bool GetControllerInvertVertical() const;
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	void SetControllerInvertVertical(bool bInvertVertical);

	// CONTROLLER INVERT HORIZONTAL
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	bool GetControllerInvertHorizontal() const;
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	void SetControllerInvertHorizontal(bool bInvertHorizontal);

	// CONTROLLER SENSITIVITY
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	float GetControllerSensitivity() const;
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	void SetControllerSensitivity(float Sensitivity);

	/************************************************************************/
	/* 							UI BINDINGS							        */
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	void BindCheckBoxToMouseInvertVertical(UCheckBox* CheckBox);
	
	
private:
	void LoadSettings();
	void SaveSettings();
	
	UPROPERTY()
	TObjectPtr<UMHPlayerSettingsSaveGame> PlayerSettings; // Shouldn't be directly accessed.
};
