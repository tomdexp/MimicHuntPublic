#pragma once

#include "CoreMinimal.h"
#include "Core/MHLivingBeing.h"
#include "MHMimicCharacter.generated.h"

UCLASS()
class MIMICHUNT_API AMHMimicCharacter : public AMHLivingBeing
{
	GENERATED_BODY()

public:
	AMHMimicCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
