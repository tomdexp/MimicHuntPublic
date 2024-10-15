#include "Utils/MHPlayerSettingsSubsystem.h"

#include "Components/CheckBox.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/MHPlayerSettingsSaveGame.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(MHPlayerSettingsSubsystem);

#define SAVE_NAME FString("PlayerSettingsSlot")

void UMHPlayerSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadSettings();
}

void UMHPlayerSettingsSubsystem::LoadSettings()
{
	LL_DBG(this, "MHPlayerSettingsSubsystem::LoadSettings Loading player settings...");
	if (UGameplayStatics::DoesSaveGameExist(SAVE_NAME, 0))
	{
		USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SAVE_NAME, 0);
		PlayerSettings = Cast<UMHPlayerSettingsSaveGame>(SaveGame);
		LL_DBG(this, "MHPlayerSettingsSubsystem::LoadSettings Player settings loaded successfully from disk.");
	}
	else
	{
		USaveGame* SaveGame = UGameplayStatics::CreateSaveGameObject(UMHPlayerSettingsSaveGame::StaticClass());
		PlayerSettings = Cast<UMHPlayerSettingsSaveGame>(SaveGame);
		UGameplayStatics::SaveGameToSlot(SaveGame, SAVE_NAME, 0);
		LL_DBG(this, "MHPlayerSettingsSubsystem::LoadSettings Player settings created and saved for the first time successfully.");
	}
}

void UMHPlayerSettingsSubsystem::SaveSettings()
{
	LL_DBG(this, "UMHPlayerSettingsSubsystem::SaveSettings Saving player settings...");
	if (PlayerSettings)
	{
		UGameplayStatics::SaveGameToSlot(PlayerSettings, SAVE_NAME, 0);
		OnPlayerSettingsChanged.Broadcast();
		LL_DBG(this, "UMHPlayerSettingsSubsystem::SaveSettings Player settings saved successfully to disk.");
	}
	else
	{
		LL_ERR(this, "UMHPlayerSettingsSubsystem::SaveSettings Player settings is null. Cannot save to disk.");
	}
}

bool UMHPlayerSettingsSubsystem::GetMouseInvertVertical() const
{
	return PlayerSettings->bMouseInvertVertical;
}

void UMHPlayerSettingsSubsystem::SetMouseInvertVertical(bool bInvertVertical)
{
	PlayerSettings->bMouseInvertVertical = bInvertVertical;
	SaveSettings();
}

bool UMHPlayerSettingsSubsystem::GetMouseInvertHorizontal() const
{
	return PlayerSettings->bMouseInvertHorizontal;
}

void UMHPlayerSettingsSubsystem::SetMouseInvertHorizontal(bool bInvertHorizontal)
{
	PlayerSettings->bMouseInvertHorizontal = bInvertHorizontal;
	SaveSettings();
}

float UMHPlayerSettingsSubsystem::GetMouseSensitivity() const
{
	return PlayerSettings->MouseSensitivity;
}

void UMHPlayerSettingsSubsystem::SetMouseSensitivity(float Sensitivity)
{
	PlayerSettings->MouseSensitivity = Sensitivity;
	SaveSettings();
}

bool UMHPlayerSettingsSubsystem::GetControllerInvertVertical() const
{
	return PlayerSettings->bControllerInvertVertical;
}

void UMHPlayerSettingsSubsystem::SetControllerInvertVertical(bool bInvertVertical)
{
	PlayerSettings->bControllerInvertVertical = bInvertVertical;
	SaveSettings();
}

bool UMHPlayerSettingsSubsystem::GetControllerInvertHorizontal() const
{
	return PlayerSettings->bControllerInvertHorizontal;
}

void UMHPlayerSettingsSubsystem::SetControllerInvertHorizontal(bool bInvertHorizontal)
{
	PlayerSettings->bControllerInvertHorizontal = bInvertHorizontal;
	SaveSettings();
}

float UMHPlayerSettingsSubsystem::GetControllerSensitivity() const
{
	return PlayerSettings->ControllerSensitivity;
}

void UMHPlayerSettingsSubsystem::SetControllerSensitivity(float Sensitivity)
{
	PlayerSettings->ControllerSensitivity = Sensitivity;
	SaveSettings();
}

FVoidCoroutine UMHPlayerSettingsSubsystem::WaitForPlayerSettingsInitialized(FLatentActionInfo LatentInfo)
{
	while (!PlayerSettings)
	{
		co_await UE5Coro::Latent::NextTick();
	}
	co_return;
}

void UMHPlayerSettingsSubsystem::BindCheckBoxToMouseInvertVertical(UCheckBox* CheckBox)
{
	if (CheckBox)
	{
		CheckBox->SetIsChecked(PlayerSettings->bMouseInvertVertical);
		CheckBox->OnCheckStateChanged.AddDynamic(this, &UMHPlayerSettingsSubsystem::SetMouseInvertVertical);
	}
	else
	{
		LL_ERR(this, "UMHPlayerSettingsSubsystem::BindCheckBoxToMouseInvertVertical CheckBox is null. Cannot bind to mouse invert vertical.");
	}
}

void UMHPlayerSettingsSubsystem::UnbindCheckBoxToMouseInvertVertical(UCheckBox* CheckBox)
{
	if (CheckBox)
	{
		CheckBox->OnCheckStateChanged.RemoveDynamic(this, &UMHPlayerSettingsSubsystem::SetMouseInvertVertical);
	}
	else
	{
		LL_ERR(this, "UMHPlayerSettingsSubsystem::UnbindCheckBoxToMouseInvertVertical CheckBox is null. Cannot unbind from mouse invert vertical.");
	}
}
