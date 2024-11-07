// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitForInteractableComponentSingleLineTrace.generated.h"

class UInteractableComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableObjectsChangedEvent, UInteractableComponent*,
                                            InteractableComponent);


/**
 * This class is an ability task that waits for an interactable component to be in range of the player.
 * When the interactable component is in range, it will be updated.
 * If there is no interactable component in range, the interactable component will be set to nullptr.
 */
UCLASS()
class MIMICHUNT_API UAbilityTask_WaitForInteractableComponentSingleLineTrace : public UAbilityTask
{
	GENERATED_BODY()

public:
	virtual void Activate() override;
	virtual void OnDestroy(bool bAbilityEnded) override;
	static void LineTrace(FHitResult& OutHitResult, const UWorld* World, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams& Params);
	void PerformTrace() const;
	void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd) const;
	void UpdateInteractableComponent(UInteractableComponent* InteractableComponent) const;

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitForInteractableComponentSingleLineTrace* WaitForInteractableComponentSingleLineTrace(UGameplayAbility* OwningAbility, FCollisionProfileName TraceProfile, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionScanRange = 100, float InteractionScanRate = 0.100, bool bShowDebug = false);
	
	UPROPERTY(BlueprintAssignable)
	FInteractableObjectsChangedEvent InteractableObjectChanged;

	FCollisionProfileName TraceProfile;
	
	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;

	float InteractionScanRange = 100;
	float InteractionScanRate = 0.100;
	bool bShowDebug = false;

	FTimerHandle TimerHandle;
};
