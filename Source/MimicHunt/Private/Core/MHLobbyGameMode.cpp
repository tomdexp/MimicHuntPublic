#include "Core/MHLobbyGameMode.h"

#include "EngineUtils.h"
#include "Core/MHCombatGameState.h"
#include "Networking/PersistentDataManager.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(MHLobbyGameMode);

void AMHLobbyGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	// Check if the PersistentDataManager is already spawned
	for (APersistentDataManager* DataManager : TActorRange<APersistentDataManager>(GetWorld()))
	{
		if (DataManager)
		{
			PersistentDataManager = DataManager;
			LL_DBG(this, "Found PersistentDataManager, reassigning it");
			return;
		}
	}

	LL_DBG(this, "Spawning PersistentDataManager because it was not found");

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;  // We never want to save team setups into the map

	UWorld* World = GetWorld();
	PersistentDataManager = World->SpawnActor<APersistentDataManager>(APersistentDataManager::StaticClass(), SpawnInfo);
	if (!PersistentDataManager)
	{
		LL_ERR(this, "Failed to spawn PersistentDataManager");
	}
	else
	{
		LL_DBG(this, "Spawned PersistentDataManager");
		// Cache it in GameState so it's easily accessible
		if (AMHGameState* GS = GetGameState<AMHGameState>())
		{
			GS->PersistentDataManager = PersistentDataManager;
			LL_DBG(this, "Cached PersistentDataManager in GameState");
		}
	}
}
