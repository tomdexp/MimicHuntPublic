#include "Core/MHPlayerState.h"

#include "MHPlayerCharacter.h"
#include "GameplayAbilitySystem/MHAbilitySystemComponent.h"
#include "GameplayAbilitySystem/AttributeSets/MHAttributeSetPlayer.h"
#include "Net/UnrealNetwork.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(PlayerState);


AMHPlayerState::AMHPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSetPlayer = CreateDefaultSubobject<UMHAttributeSetPlayer>(TEXT("AttributeSetPlayer"));

	NetUpdateFrequency = 50.0f;

	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

void AMHPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetPlayer->GetHealthAttribute()).AddUObject(this, &AMHPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetPlayer->GetMaxHealthAttribute()).AddUObject(this, &AMHPlayerState::MaxHealthChanged);
	}
}

void AMHPlayerState::OnRep_CurrentPlayerLifeType()
{
	LL_DBG(this, "AMHPlayerState::OnRep_CurrentPlayerLifeType CurrentPlayerLifeType: {0}", CurrentPlayerLifeType);
}

UAbilitySystemComponent* AMHPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UMHAttributeSetPlayer* AMHPlayerState::GetAttributeSetPlayer() const
{
	return AttributeSetPlayer;
}

void AMHPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMHPlayerState, CurrentPlayerLifeType);
}

float AMHPlayerState::GetHealth() const
{
	return AttributeSetPlayer->GetHealth();
}

float AMHPlayerState::GetMaxHealth() const
{
	return AttributeSetPlayer->GetMaxHealth();
}

void AMHPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	// If the player died, handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		if (AMHPlayerCharacter* PlayerCharacter = Cast<AMHPlayerCharacter>(GetPawn()))
		{
			PlayerCharacter->Die();
		}
	}
}

void AMHPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	// TODO : Update UI here
}

bool AMHPlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}
