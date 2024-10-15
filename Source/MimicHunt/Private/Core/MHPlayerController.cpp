#include "Core/MHPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MHPlayerCharacter.h"
#include "Data/MHPlayerData.h"

#include "Utils/LLog.h"
#include "Utils/MHPlayerSettingsSubsystem.h"

LL_FILE_CVAR(MHPlayerController);


void AMHPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!PlayerData)
	{
		LL_ERR(this, "PlayerData is nullptr, please check that it is set in the editor");
		return;
	}
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(PlayerData->InputMappingContext, 0);
	}
	if (UMHPlayerSettingsSubsystem* PlayerSettingsSubsystem = GetGameInstance()->GetSubsystem<UMHPlayerSettingsSubsystem>())
	{
		PlayerSettingsSubsystem->OnPlayerSettingsChanged.AddDynamic(this, &AMHPlayerController::PlayerSettingsChanged);
	}
}

void AMHPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (UMHPlayerSettingsSubsystem* PlayerSettingsSubsystem = GetGameInstance()->GetSubsystem<UMHPlayerSettingsSubsystem>())
	{
		PlayerSettingsSubsystem->OnPlayerSettingsChanged.RemoveDynamic(this, &AMHPlayerController::PlayerSettingsChanged);
	}
}

void AMHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!PlayerData)
	{
		LL_ERR(this, "PlayerData is nullptr, please check that it is set in the editor");
		return;
	}
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		
		EnhancedInputComponent->BindAction(PlayerData->InputActionLook, ETriggerEvent::Triggered, this, &AMHPlayerController::RequestLookAction);
		EnhancedInputComponent->BindAction(PlayerData->InputActionMove, ETriggerEvent::Triggered, this, &AMHPlayerController::RequestMoveAction);
		EnhancedInputComponent->BindAction(PlayerData->InputActionJump, ETriggerEvent::Started, this, &AMHPlayerController::RequestJumpAction);
		EnhancedInputComponent->BindAction(PlayerData->InputActionSprint, ETriggerEvent::Started, this, &AMHPlayerController::RequestSprintActionStart);
		EnhancedInputComponent->BindAction(PlayerData->InputActionSprint, ETriggerEvent::Completed, this, &AMHPlayerController::RequestSprintActionEnd);
		EnhancedInputComponent->BindAction(PlayerData->InputActionSprintToggle, ETriggerEvent::Started, this, &AMHPlayerController::RequestSprintToggleAction);
		EnhancedInputComponent->BindAction(PlayerData->InputActionCrouch, ETriggerEvent::Started, this, &AMHPlayerController::RequestCrouchActionStart);
		EnhancedInputComponent->BindAction(PlayerData->InputActionCrouch, ETriggerEvent::Completed, this, &AMHPlayerController::RequestCrouchActionEnd);
		EnhancedInputComponent->BindAction(PlayerData->InputActionCrouchToggle, ETriggerEvent::Started, this, &AMHPlayerController::RequestCrouchToggleAction);
		EnhancedInputComponent->BindAction(PlayerData->InputActionPrimaryAction, ETriggerEvent::Started, this, &AMHPlayerController::RequestPrimaryAction);
		EnhancedInputComponent->BindAction(PlayerData->InputActionSecondaryAction, ETriggerEvent::Started, this, &AMHPlayerController::RequestSecondaryAction);
	}
}

void AMHPlayerController::RequestLookAction(const FInputActionValue& InputActionValue)
{
    if (APawn* ControlledPawn = GetPawn())
    {
        const FVector2D LookVector = InputActionValue.Get<FVector2D>();
    	ControlledPawn->AddControllerYawInput(-LookVector.X);
    	ControlledPawn->AddControllerPitchInput(LookVector.Y);
    }
}

void AMHPlayerController::RequestMoveAction(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D MoveVector = InputActionValue.Get<FVector2D>();
		ControlledPawn->AddMovementInput(ControlledPawn->GetActorForwardVector(), MoveVector.Y);
		ControlledPawn->AddMovementInput(ControlledPawn->GetActorRightVector(), MoveVector.X);
	}
}

void AMHPlayerController::RequestJumpAction(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (AMHPlayerCharacter* PlayerCharacter = Cast<AMHPlayerCharacter>(ControlledPawn))
		{
			PlayerCharacter->Jump();
		}
	}
}

void AMHPlayerController::RequestSprintActionStart(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (AMHPlayerCharacter* PlayerCharacter = Cast<AMHPlayerCharacter>(ControlledPawn))
		{
			PlayerCharacter->SprintActionPressed();
		}
	}
}

void AMHPlayerController::RequestSprintActionEnd(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (AMHPlayerCharacter* PlayerCharacter = Cast<AMHPlayerCharacter>(ControlledPawn))
		{
			PlayerCharacter->SprintActionReleased();
		}
	}
}

void AMHPlayerController::RequestSprintToggleAction(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (AMHPlayerCharacter* PlayerCharacter = Cast<AMHPlayerCharacter>(ControlledPawn))
		{
			PlayerCharacter->SprintToggleActionPressed();
		}
	}
}

void AMHPlayerController::RequestCrouchActionStart(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (AMHPlayerCharacter* PlayerCharacter = Cast<AMHPlayerCharacter>(ControlledPawn))
		{
			PlayerCharacter->CrouchActionPressed();
		}
	}
}

void AMHPlayerController::RequestCrouchActionEnd(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (AMHPlayerCharacter* PlayerCharacter = Cast<AMHPlayerCharacter>(ControlledPawn))
		{
			PlayerCharacter->CrouchActionReleased();
		}
	}
}

void AMHPlayerController::RequestCrouchToggleAction(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (AMHPlayerCharacter* PlayerCharacter = Cast<AMHPlayerCharacter>(ControlledPawn))
		{
			PlayerCharacter->CrouchToggleActionPressed();
		}
	}
}

void AMHPlayerController::RequestPrimaryAction(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (AMHPlayerCharacter* PlayerCharacter = Cast<AMHPlayerCharacter>(ControlledPawn))
		{
			PlayerCharacter->PrimaryActionPressed();
		}
	}
}

void AMHPlayerController::RequestSecondaryAction(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (AMHPlayerCharacter* PlayerCharacter = Cast<AMHPlayerCharacter>(ControlledPawn))
		{
			PlayerCharacter->SecondaryActionPressed();
		}
	}
}

void AMHPlayerController::PlayerSettingsChanged()
{

}


