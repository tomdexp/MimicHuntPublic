#pragma once

#include "CoreMinimal.h"
#include "Core/MHLivingBeing.h"
#include "MHPlayerCharacter.generated.h"

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

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
