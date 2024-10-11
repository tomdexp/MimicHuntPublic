#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MHGameMode.generated.h"

/**
 * This is the GameMode inherited by all the GameMode of the game
 * WARNING : IT EXIST ONLY THE SERVER
 */
UCLASS()
class MIMICHUNT_API AMHGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMHGameMode();
	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;
};
