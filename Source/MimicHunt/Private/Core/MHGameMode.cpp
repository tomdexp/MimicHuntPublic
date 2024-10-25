#include "Core/MHGameMode.h"

#include "EngineUtils.h"
#include "Core/MHGameState.h"
#include "Utils/LLog.h"
#include "Networking/PersistentDataManager.h"

class AMHGameState;

LL_FILE_CVAR(MHGameMode);

AMHGameMode::AMHGameMode()
{
	bUseSeamlessTravel = true;
}

void AMHGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);
	
	ActorList.Add(PersistentDataManager);
}

void AMHGameMode::InitGameState()
{
	Super::InitGameState();

	// Check if the PersistentDataManager is spawned and assign it in the game state
	for (APersistentDataManager* DataManager : TActorRange<APersistentDataManager>(GetWorld()))
	{
		if (DataManager)
		{
			PersistentDataManager = DataManager; // Reassign the PersistentDataManager
			LL_DBG(this, "AMHGameMode::InitGameState : Found PersistentDataManager, reassigning it");
			
			if(AMHGameState* GS = GetGameState<AMHGameState>())
			{
				GS->PersistentDataManager = DataManager; // Cache it in GameState so it's easily accessible client-side
				LL_DBG(this, "AMHGameMode::InitGameState : Cached PersistentDataManager in GameState");
			}
			return;
		}
		LL_ERR(this, "AMHGameMode::InitGameState : Found PersistentDataManager but it was nullptr");
		return;
	}
	LL_DBG(this, "AMHGameMode::InitGameState : PersistentDataManager was not found (maybe it not spawned yet which is normal then)");
}
