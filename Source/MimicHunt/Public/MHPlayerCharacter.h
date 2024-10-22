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
	virtual void Tick(float DeltaSeconds) override;

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

	// Replicated variable for the camera's rotation (we need it for Look At animation)
	UPROPERTY(ReplicatedUsing = OnRep_CameraRotation)
	FRotator ReplicatedCameraRotation;

	// Declare the function to handle replication updates
	UFUNCTION()
	void OnRep_CameraRotation();

	UFUNCTION(Server, Unreliable)
	void Server_UpdateCameraRotation(FRotator NewRotation);
	
	void Server_UpdateCameraRotation_Implementation(FRotator NewRotation);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
	FVoidCoroutine WaitForPlayerState(FLatentActionInfo LatentInfo);

	/**
 * Returns a target that 1 meter in front of the character eyes, but only in the vertical plane.
 */
	UFUNCTION(BlueprintCallable, Category = "LivingBeing")
	FVector GetLookAtTarget() const;

	/************************************************************************/
	/* 						GAMEPLAY ABILITY SYSTEM			                */
	/************************************************************************/
	UPROPERTY()
	TObjectPtr<UMHAttributeSetPlayer> AttributeSetPlayer;
};
