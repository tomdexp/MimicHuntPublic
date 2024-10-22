#include "Core/MHLivingBeing.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySystem/MHAbilitySystemComponent.h"
#include "GameplayAbilitySystem/MHGameplayAbility.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(LivingBeing);

AMHLivingBeing::AMHLivingBeing()
{
	PrimaryActorTick.bCanEverTick = true;
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

bool AMHLivingBeing::IsAlive() const
{
	return GetHealth() > 0.0f;
}

float AMHLivingBeing::GetHealth() const
{
	if (AttributeSetLivingBeing)
	{
		return AttributeSetLivingBeing->GetHealth();
	}

	return 0.0f;
}

void AMHLivingBeing::SetHealth(float Health)
{
	if (AttributeSetLivingBeing)
	{
		AttributeSetLivingBeing->SetHealth(Health);
	}
	else
	{
		LL_ERR(this, "AttributeSetLivingBeing is nullptr for {0}", *GetName());
	}
}

float AMHLivingBeing::GetMaxHealth() const
{
	if (AttributeSetLivingBeing)
	{
		return AttributeSetLivingBeing->GetMaxHealth();
	}

	return 0.0f;
}

float AMHLivingBeing::GetSpeed() const
{
	// Get the velocity
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	return Velocity.Size();
}

float AMHLivingBeing::GetViewAngleVertical() const
{
	// Return the current pitch rotation clamped between -90 and 90 degrees
	float Pitch = GetControlRotation().Pitch;
	if (Pitch > 180.0f)
	{
		Pitch -= 360.0f;
	}
	return FMath::Clamp(Pitch, -90.0f, 90.0f);
}

float AMHLivingBeing::GetMovementDirectionX() const
{
	// Get the forward/backward velocity
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	return FVector::DotProduct(GetActorForwardVector(), Velocity);
}

float AMHLivingBeing::GetMovementDirectionY() const
{
	// Get the right/left velocity
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	return FVector::DotProduct(GetActorRightVector(), Velocity);
}

// This method is directly copied from KismetAnimationLibrary::CalculateDirection
float AMHLivingBeing::GetMovementDirection() const
{
	if (!GetVelocity().IsNearlyZero())
	{
		const FMatrix RotMatrix = FRotationMatrix(GetActorRotation());
		const FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
		const FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);
		const FVector NormalizedVel = GetVelocity().GetSafeNormal2D();

		// get a cos(alpha) of forward vector vs velocity
		const float ForwardCosAngle = static_cast<float>(FVector::DotProduct(ForwardVector, NormalizedVel));
		// now get the alpha and convert to degree
		float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

		// depending on where right vector is, flip it
		const float RightCosAngle = static_cast<float>(FVector::DotProduct(RightVector, NormalizedVel));
		if (RightCosAngle < 0.f)
		{
			ForwardDeltaDegree *= -1.f;
		}

		return ForwardDeltaDegree;
	}

	return 0.f;
}

float AMHLivingBeing::GetMovementDirectionXNormalized() const
{
	// Get the forward/backward velocity
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	Velocity.Normalize();
	return FVector::DotProduct(GetActorForwardVector(), Velocity);
}

float AMHLivingBeing::GetMovementDirectionYNormalized() const
{
	// Get the right/left velocity
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	Velocity.Normalize();
	return FVector::DotProduct(GetActorRightVector(), Velocity);
}

bool AMHLivingBeing::ShouldMove() const
{
	// Set Should Move to true only if ground speed is above a small threshold (to prevent incredibly small velocities from triggering animations) and if there is currently acceleration (input) applied.
	return GetCharacterMovement()->GetCurrentAcceleration() != FVector::ZeroVector && GetSpeed() > 3.0f;
}

UAbilitySystemComponent* AMHLivingBeing::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMHLivingBeing::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UMHGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, INDEX_NONE, this));
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void AMHLivingBeing::InitializeAttributes()
{
	if (!AbilitySystemComponent) return;

	if (!DefaultAttributes)
	{
		LL_ERR(this, "Missing DefaultAttributes for {0}. Please fill in the character's Blueprint.", *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void AMHLivingBeing::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->bStartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->bStartupEffectsApplied = true;
}
