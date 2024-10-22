#include "Core/MHLivingBeing.h"

#include "Core/MHPlayerState.h"
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
