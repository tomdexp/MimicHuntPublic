#include "GameplayAbilitySystem/AttributeSets/MHAttributeSetLivingBeing.h"

#include "Net/UnrealNetwork.h"

void UMHAttributeSetLivingBeing::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UMHAttributeSetLivingBeing, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMHAttributeSetLivingBeing, MaxHealth, COND_None, REPNOTIFY_Always);

}

void UMHAttributeSetLivingBeing::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UMHAttributeSetLivingBeing::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	// If a Max value changes, adjust current to keep Current % of Current to Max
	if (Attribute == GetMaxHealthAttribute()) // GetMaxHealthAttribute comes from the Macros defined at the top of the header
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}

}

void UMHAttributeSetLivingBeing::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMHAttributeSetLivingBeing, Health, OldHealth);
}

void UMHAttributeSetLivingBeing::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMHAttributeSetLivingBeing, MaxHealth, OldMaxHealth);
}
