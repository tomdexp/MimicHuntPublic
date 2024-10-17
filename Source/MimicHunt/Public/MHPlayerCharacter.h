#pragma once

#include "CoreMinimal.h"
#include "Core/MHLivingBeing.h"
#include "UE5Coro/UnrealTypes.h"
#include "MHPlayerCharacter.generated.h"

class UMHAttributeSetPlayer;
class UMHAbilitySystemComponent;
class UMHPlayerData;
class UCameraComponent;

UCLASS()
class MIMICHUNT_API AMHPlayerCharacter : public AMHLivingBeing
{
	GENERATED_BODY()

public:
	AMHPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DATA")
	TObjectPtr<UMHPlayerData> PlayerData;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	void UpdateMovementSpeed();
	
	void SprintActionPressed();
	void SprintActionReleased();
	void SprintToggleActionPressed();
	void CrouchActionPressed();
	void CrouchActionReleased();
	void CrouchToggleActionPressed();
	virtual void Jump() override;
	void PrimaryActionPressed();
	void PrimaryActionReleased();
	void SecondaryActionPressed();
	void SecondaryActionReleased();
	void InteractPressed();
	void InteractReleased();

	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool bNewSprinting);
	
	UPROPERTY(ReplicatedUsing = OnRep_IsSprinting)
	bool bIsSprinting = false;

	UFUNCTION()
	void OnRep_IsSprinting();

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
	FVoidCoroutine WaitForPlayerState(FLatentActionInfo LatentInfo);

	/************************************************************************/
	/* 						GAMEPLAY ABILITY SYSTEM			                */
	/************************************************************************/
	UPROPERTY()
	TObjectPtr<UMHAttributeSetPlayer> AttributeSetPlayer;
};
