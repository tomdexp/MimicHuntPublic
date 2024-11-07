#include "GameplayAbilitySystem/Interactions/AbilityTask_GrantNearbyInteraction.h"

#include "AbilitySystemComponent.h"
#include "MHCollisionChannels.h"
#include "Engine/OverlapResult.h"
#include "GameplayAbilitySystem/Interactions/InteractableComponent.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(AbilityTask_GrantNearbyInteraction);

void UAbilityTask_GrantNearbyInteraction::Activate()
{
	SetWaitingOnAvatar();

	UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionScanRate, true);
}

UAbilityTask_GrantNearbyInteraction* UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractable(
	UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate)
{
	UAbilityTask_GrantNearbyInteraction* MyObj = NewAbilityTask<UAbilityTask_GrantNearbyInteraction>(OwningAbility);
	MyObj->InteractionScanRange = InteractionScanRange;
	MyObj->InteractionScanRate = InteractionScanRate;
	return MyObj;
}

void UAbilityTask_GrantNearbyInteraction::OnDestroy(bool AbilityEnded)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
	}

	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_GrantNearbyInteraction::QueryInteractables()
{
    UWorld* World = GetWorld();
    AActor* ActorOwner = GetAvatarActor();

    if (!World || !ActorOwner)
    {
        UE_LOG(LogTemp, Warning, TEXT("QueryInteractables: World or ActorOwner is null."));
        return;
    }

    FCollisionQueryParams Params(SCENE_QUERY_STAT(UAbilityTask_GrantNearbyInteraction), false);

    TArray<FOverlapResult> OverlapResults;
    FVector Location = ActorOwner->GetActorLocation();
    FQuat Rotation = FQuat::Identity;
    float ScanRange = InteractionScanRange;

    #if ENABLE_DRAW_DEBUG
    DrawDebugSphere(
        World,
        Location,
        ScanRange,
        24, // Number of segments
        FColor::Green,
        false,
        InteractionScanRate // Duration in seconds
    );
    #endif

    // Perform the overlap query using the "Interactable" Trace Channel
    bool bHasOverlap = World->OverlapMultiByChannel(
        OverlapResults,
        Location,
        Rotation,
        MH_TRACE_CHANNEL_INTERACTION,
        FCollisionShape::MakeSphere(ScanRange),
        Params
    );

    if (bHasOverlap && OverlapResults.Num() > 0)
    {
        TArray<UInteractableComponent*> InteractableComponents;
        InteractableComponents.Reserve(OverlapResults.Num()); // Reserve space to optimize

        // Iterate through each overlap result and collect InteractableComponents
        for (const FOverlapResult& Result : OverlapResults)
        {
            if (AActor* OverlappingActor = Result.GetActor())
            {
                // Get all UInteractableComponent instances attached to the actor
                TArray<UInteractableComponent*> Components;
                OverlappingActor->GetComponents<UInteractableComponent>(Components);

                for (UInteractableComponent* Component : Components)
                {
                    if (Component)
                    {
                        InteractableComponents.Add(Component);
                    }
                }
            }
        }

        if (InteractableComponents.Num() == 0)
        {
           // UE_LOG(LogTemp, Log, TEXT("No valid interactable components found."));
            return;
        }

        // Gather interaction options from each InteractableComponent
        for (UInteractableComponent* InteractableComponent : InteractableComponents)
        {
            if (InteractableComponent)
            {
                // Get the UInteractableComponent.InteractionAbility and store it efficiently in the TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache;
                if (!InteractableComponent->InteractionAbility)
                {
                    LL_WRN(this, "UAbilityTask_GrantNearbyInteraction::QueryInteractables : InteractableComponent has no InteractionAbility.");
                    return;
                }
                FObjectKey ObjectKey(InteractableComponent->InteractionAbility);
                if (!InteractionAbilityCache.Find(ObjectKey))
                {
                    FGameplayAbilitySpec Spec(InteractableComponent->InteractionAbility, 1, INDEX_NONE, this);
                    FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
                    InteractionAbilityCache.Add(ObjectKey, Handle);
                }
            }
        }
    }
}
