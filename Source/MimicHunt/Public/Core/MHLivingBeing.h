#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MHLivingBeing.generated.h"

/**
 * This is the base class for all the living beings of the game
 * It has two C++ children :
 * - AMHPlayerCharacter : The player character
 * - AMHMimicCharacter : The mimic character
 *
 * Its where we put methods and attributes that are common to all the living beings of the game
 * Like utilities methods for animations (GetSpeed(), GetDirection(), etc)
 */
UCLASS()
class MIMICHUNT_API AMHLivingBeing : public ACharacter
{
	GENERATED_BODY()

public:
	AMHLivingBeing();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
