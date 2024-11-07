#include "Core/MHLivingBeing.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/SPIRV-Headers/include/spirv/unified1/spirv.h>

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySystem/MHAbilitySystemComponent.h"
#include "GameplayAbilitySystem/MHGameplayAbility.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(LivingBeing);

AMHLivingBeing::AMHLivingBeing()
{
	PrimaryActorTick.bCanEverTick = true;
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Effect.RemoveOnDeath"));
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

ECardinalDirection4 AMHLivingBeing::GetCurrentCardinalDirection4() const
{
	FVector ForwardVector = GetActorForwardVector();
    
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.0f;

	if (Velocity.IsNearlyZero())
	{
		return ECardinalDirection4::None;
	}

	Velocity.Normalize();

	float DotProduct = FVector::DotProduct(ForwardVector, Velocity);
    
	DotProduct = FMath::Clamp(DotProduct, -1.0f, 1.0f);

	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

	if (FVector::DotProduct(GetActorRightVector(), Velocity) < 0.0f)
	{
		Angle = 360.0f - Angle;
	}

	if (Angle >= 45.0f && Angle < 135.0f)
	{
		return ECardinalDirection4::Right;
	}
	if (Angle >= 135.0f && Angle < 225.0f)
	{
		return ECardinalDirection4::Backward;
	}
	if (Angle >= 225.0f && Angle < 315.0f)
	{
		return ECardinalDirection4::Left;
	}

	return ECardinalDirection4::Forward;
}

ECardinalDirection8 AMHLivingBeing::GetCurrentCardinalDirection8() const
{
    FVector ForwardVector = GetActorForwardVector();
    FVector RightVector = GetActorRightVector();
    
    FVector Velocity = GetVelocity();
    Velocity.Z = 0.0f;

    if (Velocity.IsNearlyZero())
    {
        return ECardinalDirection8::None;
    }

    Velocity.Normalize();

    float DotForward = FVector::DotProduct(ForwardVector, Velocity);
    float DotRight = FVector::DotProduct(RightVector, Velocity);
    
    DotForward = FMath::Clamp(DotForward, -1.0f, 1.0f);
    DotRight = FMath::Clamp(DotRight, -1.0f, 1.0f);

    float Angle = FMath::RadiansToDegrees(FMath::Acos(DotForward));

    if (DotRight < 0.0f)
    {
        Angle = 360.0f - Angle;
    }

    if (Angle >= 22.5f && Angle < 67.5f)
    {
        return ECardinalDirection8::ForwardRight;
    }
    if (Angle >= 67.5f && Angle < 112.5f)
    {
        return ECardinalDirection8::Right;
    }
    if (Angle >= 112.5f && Angle < 157.5f)
    {
        return ECardinalDirection8::BackwardRight;
    }
    if (Angle >= 157.5f && Angle < 202.5f)
    {
        return ECardinalDirection8::Backward;
    }
    if (Angle >= 202.5f && Angle < 247.5f)
    {
        return ECardinalDirection8::BackwardLeft;
    }
    if (Angle >= 247.5f && Angle < 292.5f)
    {
        return ECardinalDirection8::Left;
    }
    if (Angle >= 292.5f && Angle < 337.5f)
    {
        return ECardinalDirection8::ForwardLeft;
    }

    return ECardinalDirection8::Forward;
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
		FGameplayAbilitySpecHandle AbilityHandle;
		FTagBoundGameplayAbility TagBoundAbility;

		switch (StartupAbility.GetDefaultObject()->ActivationType)
		{
		case EMHAbilityActivationType::Undefined:
			LL_WRN(this, "AMHLivingBeing::AddCharacterAbilities : Undefined ActivationType for {0}. Please fill in the character's Blueprint.", *GetName());
			break;
		case EMHAbilityActivationType::BindToInput:
			LL_DBG(this, "AMHLivingBeing::AddCharacterAbilities : adding {0} with InputID {1}", *StartupAbility->GetName(), StartupAbility.GetDefaultObject()->ActivationInputType);
			AbilityHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, INDEX_NONE, this));
			TagBoundAbility.AbilityHandle = AbilityHandle;
			TagBoundAbility.GameplayTagStart = StartupAbility.GetDefaultObject()->EventTag;
			TagBoundAbility.GameplayTagCancel = StartupAbility.GetDefaultObject()->EventTagCancel;
			AbilitySystemComponent->TagBoundAbilities.Add(TagBoundAbility);
			break;
		case EMHAbilityActivationType::BindToTagEvent:
			LL_DBG(this, "AMHLivingBeing::AddCharacterAbilities : adding {0} added with EventTag {1}", *StartupAbility->GetName(), *StartupAbility.GetDefaultObject()->EventTag.ToString());
			AbilityHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, INDEX_NONE, this));
			break;
		case EMHAbilityActivationType::None:
			LL_DBG(this, "AMHLivingBeing::AddCharacterAbilities : adding {0} with None", *StartupAbility->GetName());
			AbilityHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, INDEX_NONE, this));
			break;
		}

		if (StartupAbility.GetDefaultObject()->bActivateOnSpawn)
		{
			if (!AbilityHandle.IsValid())
			{
				LL_WRN(this, "AMHLivingBeing::AddCharacterAbilities : Failed to activate ability on spawn {0}, because AbilityHandle was null", *StartupAbility->GetName());
			}
			else
			{
				LL_DBG(this, "AMHLivingBeing::AddCharacterAbilities : activating {0} because bActivateOnSpawn", *StartupAbility->GetName());
				AbilitySystemComponent->TryActivateAbility(AbilityHandle);
			}
		}
	}
	
	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void AMHLivingBeing::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || !AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
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

void AMHLivingBeing::Die()
{
	// Only runs on Server
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	OnLivingBeingDied.Broadcast(this);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	// TODO : Play death animation here if needed
	// If so, the FinishDying method should be called at the end of the animation
	FinishDying();
}

void AMHLivingBeing::FinishDying()
{
	LL_DBG(this, "AMHLivingBeing::FinishDying : {0} is dead", *GetName());
	Destroy();
}
