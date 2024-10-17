#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MHAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(MimicHunt), meta=(BlueprintSpawnableComponent))
class MIMICHUNT_API UMHAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UMHAbilitySystemComponent();
};
