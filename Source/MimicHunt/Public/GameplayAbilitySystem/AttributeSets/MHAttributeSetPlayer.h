#pragma once

#include "CoreMinimal.h"
#include "MHAttributeSetLivingBeing.h"
#include "MHAttributeSetPlayer.generated.h"

/**
 * This is where the player's attributes will be defined.
 * All player specific attributes will be defined here.
 * Like : Money, Level, Ammunition, etc.
 */
UCLASS()
class MIMICHUNT_API UMHAttributeSetPlayer : public UMHAttributeSetLivingBeing
{
	GENERATED_BODY()
};
