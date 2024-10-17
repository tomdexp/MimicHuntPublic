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
	virtual void BeginPlay() override;

	/************************************************************************/
	/* 						GAMEPLAY ABILITY SYSTEM			                */
	/************************************************************************/
	UPROPERTY()
	TObjectPtr<class UMHAttributeSetMimic> AttributeSetMimic;
};
