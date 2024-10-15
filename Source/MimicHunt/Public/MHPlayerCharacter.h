#pragma once

#include "CoreMinimal.h"
#include "Core/MHLivingBeing.h"
#include "MHPlayerCharacter.generated.h"

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
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DATA")
	TObjectPtr<UMHPlayerData> PlayerData;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void SprintActionPressed();
	void SprintActionReleased();
	void SprintToggleActionPressed();
	void CrouchActionPressed();
	void CrouchActionReleased();
	void CrouchToggleActionPressed();
	virtual void Jump() override;
	void PrimaryActionPressed();
	void SecondaryActionPressed();
};
