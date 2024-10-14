#pragma once

#include "CoreMinimal.h"
#include "Core/MHLivingBeing.h"
#include "MHPlayerCharacter.generated.h"

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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void SprintActionPressed();
	void CrouchActionPressed();
	virtual void Jump() override;
	
};
