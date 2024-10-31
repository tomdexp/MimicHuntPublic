#include "MHPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/MHGameInstance.h"
#include "Core/MHGameMode.h"
#include "Core/MHPlayerState.h"
#include "Data/MHPlayerData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameplayAbilitySystem/MHAbilitySystemComponent.h"
#include "GameplayAbilitySystem/AttributeSets/MHAttributeSetLivingBeing.h"
#include "GameplayAbilitySystem/AttributeSets/MHAttributeSetPlayer.h"
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
	FirstPersonCameraComponent->SetIsReplicated(true); // Replicate the camera component

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonCharacterMeshComponent"));
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);
	FirstPersonMeshComponent->SetupAttachment(FirstPersonCameraComponent);
	FirstPersonMeshComponent->bCastDynamicShadow = false;
	FirstPersonMeshComponent->CastShadow = false;
	// FirstPersonMeshComponent->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	FirstPersonMeshComponent->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	//AkOdinInputComponent = CreateDefaultSubobject<UAkOdinInputComponent>(TEXT("AkOdinInputComponent"));
	

	// Hide the main mesh in the 1st person view
	GetMesh()->SetOwnerNoSee(true);
	
	// Enable the character to crouch
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void AMHPlayerCharacter::BeginPlay()
{
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
	Super::BeginPlay();
}

void AMHPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// Only update if we are the locally controlled player
	if (IsLocallyControlled())
	{
		FRotator NewRotation = FirstPersonCameraComponent->GetComponentRotation();

		// Check if the rotation has actually changed before sending it
		if (!NewRotation.Equals(ReplicatedCameraRotation, 1.0f)) // A tolerance of 1.0 degree for optimization
		{
			Server_UpdateCameraRotation(NewRotation);
		}
	}
}


void AMHPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMHPlayerCharacter, bIsSprinting);
	DOREPLIFETIME(AMHPlayerCharacter, ReplicatedCameraRotation);
}

void AMHPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Server only
void AMHPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AMHPlayerState* PS = GetPlayerState<AMHPlayerState>())
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UMHAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// Since AttributeSetPlayer is a subclass of AttributeSetLivingBeing
		AttributeSetLivingBeing = Cast<UMHAttributeSetLivingBeing>(PS->GetAttributeSetPlayer());
		AttributeSetPlayer = PS->GetAttributeSetPlayer();

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();
		
		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		//SetMana(GetMaxMana());
		//SetStamina(GetMaxStamina());

		// End respawn specific things
		AddStartupEffects();

		AddCharacterAbilities();
	}
}

void AMHPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AMHPlayerState* PS = GetPlayerState<AMHPlayerState>())
	{
		// Set the ASC for clients. Server does this in PossessedBy.
		AbilitySystemComponent = Cast<UMHAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// Since AttributeSetPlayer is a subclass of AttributeSetLivingBeing
		AttributeSetLivingBeing = Cast<UMHAttributeSetLivingBeing>(PS->GetAttributeSetPlayer());
		AttributeSetPlayer = PS->GetAttributeSetPlayer();

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	}
}

void AMHPlayerCharacter::FinishDying()
{
	if (HasAuthority())
	{
		if (AMHGameMode* GameMode = Cast<AMHGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->PlayerCharacterDied(GetController());
		}
	}

	Super::FinishDying();
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
	// Send gameplay event to the ability system, the tag is "Event.Ability.SimpleRaycastDamage"
	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Ability.SimpleRaycastDamage"));
	int32 TriggeredAbilitiesCount = AbilitySystemComponent->HandleGameplayEvent(EventData.EventTag, &EventData);
	LL_DBG(this, "AMHPlayerCharacter::PrimaryActionPressed : TriggeredAbilitiesCount {0}", TriggeredAbilitiesCount);
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

void AMHPlayerCharacter::OnRep_CameraRotation()
{
	// Update the camera's rotation on the client when replication occurs
	if (FirstPersonCameraComponent)
	{
		FirstPersonCameraComponent->SetWorldRotation(ReplicatedCameraRotation);
	}
}

void AMHPlayerCharacter::Server_UpdateCameraRotation_Implementation(FRotator NewRotation)
{
	// Update the replicated camera rotation on the server
	ReplicatedCameraRotation = NewRotation;

	// Update the camera's rotation on the server's version of the character (might be not needed)
	if (FirstPersonCameraComponent)
	{
		FirstPersonCameraComponent->SetWorldRotation(ReplicatedCameraRotation);
	}
}

FVoidCoroutine AMHPlayerCharacter::WaitForPlayerState(FLatentActionInfo LatentInfo)
{
	while (!GetPlayerState())
	{
		co_await UE5Coro::Latent::NextTick();
	}
	co_return;
}

FVoidCoroutine AMHPlayerCharacter::WaitForPlayerController(FLatentActionInfo LatentInfo)
{
	while (!GetController())
	{
		co_await UE5Coro::Latent::NextTick();
	}
	co_return;
}

FVector AMHPlayerCharacter::GetLookAtTarget() const
{
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector End = Start + (ForwardVector * 100.0f); // 1 meter in front

	return End;
}
