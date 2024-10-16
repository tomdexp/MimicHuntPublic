#include "MHPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/MHPlayerData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(MHPlayerCharacter);

AMHPlayerCharacter::AMHPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Create the camera component, and attach it to the capsule component (like in the template=
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCameraComponent"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bCameraMeshHiddenInGame = false; // Show the camera mesh in game

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonCharacterMeshComponent"));
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);
	FirstPersonMeshComponent->SetupAttachment(FirstPersonCameraComponent);
	FirstPersonMeshComponent->bCastDynamicShadow = false;
	FirstPersonMeshComponent->CastShadow = false;
	// FirstPersonMeshComponent->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	FirstPersonMeshComponent->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// Hide the main mesh in the 1st person view
	GetMesh()->SetOwnerNoSee(true);
	
	// Enable the character to crouch
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void AMHPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind the player data to the character
	if (!PlayerData->IsValidLowLevel())
	{
		LL_ERR(this, "PlayerData is nullptr, please check that it is set in the editor inside the BP_MHPlayerCharacter");
	}
	else
	{
		// Set the player walk speed
		GetCharacterMovement()->MaxWalkSpeed = PlayerData->WalkSpeed;
		// Set the player crouch speed
		GetCharacterMovement()->MaxWalkSpeedCrouched = PlayerData->CrouchSpeed;
		// Set the player jump velocity
		GetCharacterMovement()->JumpZVelocity = PlayerData->JumpVelocity;
	}
}


void AMHPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMHPlayerCharacter, bIsSprinting);
}

void AMHPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMHPlayerCharacter::UpdateMovementSpeed()
{
	LL_DBG(this, "AMHPlayerCharacter::UpdateMovementSpeed");
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? PlayerData->SprintSpeed : PlayerData->WalkSpeed;
}

void AMHPlayerCharacter::SprintActionPressed()
{
	LL_DBG(this, "AMHPlayerCharacter::SprintActionPressed");
	if (!bIsSprinting)
	{
		bIsSprinting = true;
		OnRep_IsSprinting();
		ServerSetSprinting(true);
	}
}

void AMHPlayerCharacter::SprintActionReleased()
{
	LL_DBG(this, "AMHPlayerCharacter::SprintActionReleased");
	if (bIsSprinting)
	{
		bIsSprinting = false;
		OnRep_IsSprinting();
		ServerSetSprinting(false);
	}
}

void AMHPlayerCharacter::SprintToggleActionPressed()
{
	LL_DBG(this, "AMHPlayerCharacter::SprintToggleActionPressed");
	if (bIsSprinting)
	{
		bIsSprinting = false;
		OnRep_IsSprinting();
		ServerSetSprinting(false);
	}
	else
	{
		bIsSprinting = true;
		OnRep_IsSprinting();
		ServerSetSprinting(true);
	}
}

void AMHPlayerCharacter::CrouchActionPressed()
{
	LL_DBG(this, "AMHPlayerCharacter::CrouchActionPressed");
	Crouch();
}

void AMHPlayerCharacter::CrouchActionReleased()
{
	LL_DBG(this, "AMHPlayerCharacter::CrouchActionReleased");
	UnCrouch();
}

void AMHPlayerCharacter::CrouchToggleActionPressed()
{
	LL_DBG(this, "AMHPlayerCharacter::CrouchToggleActionPressed");
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AMHPlayerCharacter::Jump()
{
	Super::Jump();
	LL_DBG(this, "AMHPlayerCharacter::Jump");
}

void AMHPlayerCharacter::PrimaryActionPressed()
{
	LL_DBG(this, "AMHPlayerCharacter::PrimaryActionPressed");
}

void AMHPlayerCharacter::PrimaryActionReleased()
{
	LL_DBG(this, "AMHPlayerCharacter::PrimaryActionReleased");
}

void AMHPlayerCharacter::SecondaryActionPressed()
{
	LL_DBG(this, "AMHPlayerCharacter::SecondaryActionPressed");
}

void AMHPlayerCharacter::SecondaryActionReleased()
{
	LL_DBG(this, "AMHPlayerCharacter::SecondaryActionReleased");
}

void AMHPlayerCharacter::InteractPressed()
{
	LL_DBG(this, "AMHPlayerCharacter::InteractPressed {0}",GetPlayerState()->GetPlayerId());
	// Get all players in the GameState
	for (APlayerState* ps : GetWorld()->GetGameState()->PlayerArray)
	{
		if (ps->GetPlayerId() != GetPlayerState()->GetPlayerId())
		{
			LL_DBG(this, "AMHPlayerCharacter::InteractPressed : Player {0} is not the same as {1}", ps->GetPlayerId(), GetPlayerState()->GetPlayerId());
		}
	}
}

void AMHPlayerCharacter::InteractReleased()
{
	LL_DBG(this, "AMHPlayerCharacter::InteractReleased");
}

void AMHPlayerCharacter::ServerSetSprinting_Implementation(bool bNewSprinting)
{
	LL_DBG(this, "AMHPlayerCharacter::ServerSetSprinting_Implementation");
	bIsSprinting = bNewSprinting;
	OnRep_IsSprinting();
}

void AMHPlayerCharacter::OnRep_IsSprinting()
{
	LL_DBG(this, "AMHPlayerCharacter::OnRep_IsSprinting");
	UpdateMovementSpeed();
}
