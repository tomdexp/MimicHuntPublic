// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Interactions/MHGameplayAbility_Interact.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySystem/Interactions/AbilityTask_GrantNearbyInteraction.h"
#include "GameplayAbilitySystem/Interactions/InteractableComponent.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(MHGameplayAbility_Interact);

UMHGameplayAbility_Interact::UMHGameplayAbility_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bActivateOnSpawn = true;

	AbilityInteractionActivateTag = FGameplayTag::RequestGameplayTag(FName("Event.Ability.Interaction.Activate"));
}

void UMHGameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (AbilitySystem && AbilitySystem->GetOwnerRole() == ROLE_Authority)
	{
		UAbilityTask_GrantNearbyInteraction* Task = UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractable(this, InteractionScanRange, InteractionScanRate);
		Task->ReadyForActivation();
	}
}

void UMHGameplayAbility_Interact::UpdateInteractable(UInteractableComponent* Interactable)
{
	if (Interactable)
	{
		//LL_DBG(this, "UMHGameplayAbility_Interact::UpdateInteractable : Setting CurrentInteractable to {0}.", *Interactable->GetName());
		CurrentInteractable = Interactable;
	}
	else
	{
		//LL_DBG(this, "UMHGameplayAbility_Interact::UpdateInteractable : Interactable is null. Resetting CurrentInteractable.");
		CurrentInteractable.Reset();
	}
}

void UMHGameplayAbility_Interact::TriggerInteraction()
{
	if (!CurrentInteractable.IsValid())
	{
		LL_DBG(this, "UMHGameplayAbility_Interact::TriggerInteraction : No interactable component to interact with. Aborting.");
		return;
	}

	if (UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo())
	{
		LL_DBG(this, "UMHGameplayAbility_Interact::TriggerInteraction : Found ability system component for triggering interaction.");
		AActor* Instigator = GetAvatarActorFromActorInfo();
		AActor* InteractableTargetActor = CurrentInteractable->GetOwner();

		// Allow the target to customize the event data we're about to pass in, in case the ability needs custom data
		// that only the actor knows.
		FGameplayEventData Payload;
		Payload.EventTag = AbilityInteractionActivateTag;
		Payload.Instigator = Instigator;
		Payload.Target = InteractableTargetActor;

		// If needed we allow the interactable target to manipulate the event data so that for example, a button on the wall
		// may want to specify a door actor to execute the ability on, so it might choose to override Target to be the
		// door actor.
		// InteractionOption.InteractableTarget->CustomizeInteractionEventData(TAG_Ability_Interaction_Activate, Payload);

		// Grab the target actor off the payload we're going to use it as the 'avatar' for the interaction, and the
		// source InteractableTarget actor as the owner actor.
		AActor* TargetActor = const_cast<AActor*>(ToRawPtr(Payload.Target));

		// The actor info needed for the interaction.
		FGameplayAbilityActorInfo ActorInfo;

		bool bSuccess = false;
		
		if (CurrentInteractable->TargetAbilitySystem.IsValid())
		{
			// Trigger the ability using event tag on the target ability system.
			LL_DBG(this, "UMHGameplayAbility_Interact::TriggerInteraction : Triggering ability ({0}) on target ability system.", *CurrentInteractable->InteractionAbility->GetName());

			ActorInfo.InitFromActor(InteractableTargetActor, TargetActor, CurrentInteractable->TargetAbilitySystem.Get());

			// Create the ability spec handle from the target ability system.
			FGameplayAbilitySpec* AbilitySpec = CurrentInteractable->TargetAbilitySystem->FindAbilitySpecFromClass(CurrentInteractable->InteractionAbility);
			bSuccess = CurrentInteractable->TargetAbilitySystem->TriggerAbilityFromGameplayEvent(
				AbilitySpec->Handle,
				&ActorInfo,
				AbilityInteractionActivateTag,
				&Payload,
				*CurrentInteractable->TargetAbilitySystem
			);
		}
		else if (CurrentInteractable->InteractionAbility)
		{
			// Trigger the ability using event tag on the interacting player's ability system.
			LL_DBG(this, "UMHGameplayAbility_Interact::TriggerInteraction : Triggering ability ({0}) on player ability system.", *CurrentInteractable->InteractionAbility->GetName());

			ActorInfo.InitFromActor(InteractableTargetActor, TargetActor, AbilitySystem);
			
			// Create the ability spec handle from the player ability system.
			FGameplayAbilitySpec* AbilitySpec = AbilitySystem->FindAbilitySpecFromClass(CurrentInteractable->InteractionAbility);
			bSuccess = AbilitySystem->TriggerAbilityFromGameplayEvent(
				AbilitySpec->Handle,
				&ActorInfo,
				AbilityInteractionActivateTag,
				&Payload,
				*AbilitySystem
			);
		}
		else
		{
			LL_WRN(this, "UMHGameplayAbility_Interact::TriggerInteraction : No interaction ability found on interactable component.");
		}

		if (!bSuccess)
		{
			LL_WRN(this, "UMHGameplayAbility_Interact::TriggerInteraction : Failed to trigger interaction ability.");
		}
	}
	else
	{
		LL_WRN(this, "UMHGameplayAbility_Interact::TriggerInteraction : No ability system component found with GetAbilitySystemComponentFromActorInfo()");
	}
}
