#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MHGameHUD.generated.h"

/**
 * This class should be derived in Blueprint to create the HUD for the game. (Lobby and Combat)
 */
UCLASS()
class MIMICHUNT_API AMHGameHUD : public AHUD
{
	GENERATED_BODY()
};
