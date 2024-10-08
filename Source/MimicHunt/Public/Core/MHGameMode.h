#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MHGameMode.generated.h"

/**
 * This is the gamemode used for the real gameplay (lobby and in game).
 * It's supposed to be inherited with a blueprint
 */
UCLASS()
class MIMICHUNT_API AMHGameMode : public AGameModeBase
{
	GENERATED_BODY()
};
