// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Interactions/AbilityTask_WaitForInteractableComponentSingleLineTrace.h"

#include "GameplayAbilitySystem/Interactions/InteractableComponent.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(AbilityTask_WaitForInteractableComponentSingleLineTrace);


void UAbilityTask_WaitForInteractableComponentSingleLineTrace::Activate()
{
	SetWaitingOnAvatar();

	UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::PerformTrace, InteractionScanRate, true);
}

void UAbilityTask_WaitForInteractableComponentSingleLineTrace::OnDestroy(bool bAbilityEnded)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	Super::OnDestroy(bAbilityEnded);
}

void UAbilityTask_WaitForInteractableComponentSingleLineTrace::LineTrace(FHitResult& OutHitResult, const UWorld* World,
	const FVector& Start, const FVector& End, const FName ProfileName, const FCollisionQueryParams& Params)
{
	check(World);

	OutHitResult = FHitResult();
	TArray<FHitResult> HitResults;
	World->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, Params);

	OutHitResult.TraceStart = Start;
	OutHitResult.TraceEnd = End;

	if (HitResults.Num() > 0)
	{
		OutHitResult = HitResults[0];
	}
}

UAbilityTask_WaitForInteractableComponentSingleLineTrace* UAbilityTask_WaitForInteractableComponentSingleLineTrace::
WaitForInteractableComponentSingleLineTrace(UGameplayAbility* OwningAbility, FCollisionProfileName TraceProfile,
	FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionScanRange, float InteractionScanRate,
	bool bShowDebug)
{
	UAbilityTask_WaitForInteractableComponentSingleLineTrace* Task = NewAbilityTask<UAbilityTask_WaitForInteractableComponentSingleLineTrace>(OwningAbility);
	Task->InteractionScanRange = InteractionScanRange;
	Task->InteractionScanRate = InteractionScanRate;
	Task->StartLocation = StartLocation;
	Task->TraceProfile = TraceProfile;
	Task->bShowDebug = bShowDebug;

	return Task;
}

void UAbilityTask_WaitForInteractableComponentSingleLineTrace::PerformTrace() const
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!AvatarActor)
	{
		return;
	}

	UWorld* World = GetWorld();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);

	const bool bTraceComplex = false;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(UAbilityTask_WaitForInteractableComponentSingleLineTrace), bTraceComplex);
	Params.AddIgnoredActors(ActorsToIgnore);

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;
	AimWithPlayerController(AvatarActor, Params, TraceStart, InteractionScanRange, OUT TraceEnd);

	FHitResult OutHitResult;
	LineTrace(OutHitResult, World, TraceStart, TraceEnd, TraceProfile.Name, Params);

	// Each actors can have multiple interactable components
	// So we must iterate through all of them and pick the first one that has bIsInteractable set to true

	bool bFoundInteractableComponent = false;

	if (const AActor* HitActor = OutHitResult.GetActor())
	{
		TArray<UInteractableComponent*> InteractableComponents;
		HitActor->GetComponents<UInteractableComponent>(InteractableComponents);

		for (UInteractableComponent* InteractableComponent : InteractableComponents)
		{
			if (InteractableComponent && InteractableComponent->bIsInteractable)
			{
				// Found an interactable component
				bFoundInteractableComponent = true;
				UpdateInteractableComponent(InteractableComponent);
			}
		}
	}

	if (!bFoundInteractableComponent)
	{
		// No hit actor
		UpdateInteractableComponent(nullptr);
	}
	
#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		FColor DebugColor = OutHitResult.bBlockingHit ? FColor::Red : FColor::Green;
		if (OutHitResult.bBlockingHit)
		{
			DrawDebugLine(World, TraceStart, OutHitResult.Location, DebugColor, false, InteractionScanRate);
			DrawDebugSphere(World, OutHitResult.Location, 5, 16, DebugColor, false, InteractionScanRate);
		}
		else
		{
			DrawDebugLine(World, TraceStart, TraceEnd, DebugColor, false, InteractionScanRate);
		}
	}
#endif // ENABLE_DRAW_DEBUG
}

void UAbilityTask_WaitForInteractableComponentSingleLineTrace::AimWithPlayerController(const AActor* InSourceActor,
	FCollisionQueryParams Params, const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd) const
{
	if (!Ability) // Server and launching client only
	{
		return;
	}

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	check(PC);

	FVector ViewStart;
	FRotator ViewRot;
	PC->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	FVector ViewEnd = ViewStart + (ViewDir * MaxRange);
	
	FHitResult HitResult;
	LineTrace(HitResult, InSourceActor->GetWorld(), ViewStart, ViewEnd, TraceProfile.Name, Params);

	const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(TraceStart, HitResult.Location) <= (MaxRange * MaxRange));

	const FVector AdjustedEnd = (bUseTraceResult) ? HitResult.Location : ViewEnd;

	FVector AdjustedAimDir = (AdjustedEnd - TraceStart).GetSafeNormal();
	if (AdjustedAimDir.IsZero())
	{
		AdjustedAimDir = ViewDir;
	}

	if (bUseTraceResult)
	{
		FVector OriginalAimDir = (ViewEnd - TraceStart).GetSafeNormal();

		if (!OriginalAimDir.IsZero())
		{
			// Convert to angles and use original pitch
			const FRotator OriginalAimRot = OriginalAimDir.Rotation();

			FRotator AdjustedAimRot = AdjustedAimDir.Rotation();
			AdjustedAimRot.Pitch = OriginalAimRot.Pitch;

			AdjustedAimDir = AdjustedAimRot.Vector();
		}
	}

	OutTraceEnd = TraceStart + (AdjustedAimDir * MaxRange);
}

void UAbilityTask_WaitForInteractableComponentSingleLineTrace::UpdateInteractableComponent(UInteractableComponent* InteractableComponent) const
{
	InteractableObjectChanged.Broadcast(InteractableComponent);
}

