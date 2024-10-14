#include "MHPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
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
	//FirstPersonMeshComponent->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	FirstPersonMeshComponent->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void AMHPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMHPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMHPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMHPlayerCharacter::SprintActionPressed()
{
	LL_DBG(this, "AMHPlayerCharacter::SprintActionPressed");
}

void AMHPlayerCharacter::CrouchActionPressed()
{
	LL_DBG(this, "AMHPlayerCharacter::CrouchActionPressed");
}

void AMHPlayerCharacter::Jump()
{
	Super::Jump();
	LL_DBG(this, "AMHPlayerCharacter::Jump");
}

