#pragma once

#include "CoreMinimal.h"
#include "MHAttributeSetLivingBeing.h"
#include "MHAttributeSetMimic.generated.h"

/**
 * This is the attribute set for the Mimic. It inherits from MHAttributeSetLivingBeing.
 * All Mimic's specific attributes should be defined here.
 */
UCLASS()
class MIMICHUNT_API UMHAttributeSetMimic : public UMHAttributeSetLivingBeing
{
	GENERATED_BODY()
};
