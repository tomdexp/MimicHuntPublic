#include "MHMimicCharacter.h"

#include "GameplayAbilitySystem/MHAbilitySystemComponent.h"
#include "GameplayAbilitySystem/AttributeSets/MHAttributeSetMimic.h"


AMHMimicCharacter::AMHMimicCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent = CreateDefaultSubobject<UMHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSetMimic = CreateDefaultSubobject<UMHAttributeSetMimic>(TEXT("AttributeSetMimic"));
	AttributeSetLivingBeing = AttributeSetMimic;
}

void AMHMimicCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		InitializeAttributes();
		AddStartupEffects();
		AddCharacterAbilities();
	}
}
