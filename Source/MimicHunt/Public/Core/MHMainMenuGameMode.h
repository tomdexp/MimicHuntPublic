#pragma once

#include "CoreMinimal.h"
#include "MHGameMode.h"
#include "GameFramework/GameModeBase.h"
#include "MHMainMenuGameMode.generated.h"

/**
 * This GameMode is used for the main menu of the game as an override for the L_MainMenu map.
 * - Why ?
 * - Because otherwise the game would use the default GameMode which is not what we want since it will spawn the player and the HUD. But we don't want that in the main menu.
 */
UCLASS()
class MIMICHUNT_API AMHMainMenuGameMode : public AMHGameMode
{
	GENERATED_BODY()
};
