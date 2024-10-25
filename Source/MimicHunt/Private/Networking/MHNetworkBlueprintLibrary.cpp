#include "Networking/MHNetworkBlueprintLibrary.h"

#include "EngineUtils.h"
#include "Core/MHGameState.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(MHNetworkBlueprintLibrary);


void UMHNetworkBlueprintLibrary::ServerTravelToLevel(const UObject* WorldContextObject, const FString& LevelName)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		LL_DBG(World, "UMHNetworkBlueprintLibrary::ServerTravelToLevel : ServerTravel to {0}", LevelName);
		World->ServerTravel(LevelName);
	}
}

APersistentDataManager* UMHNetworkBlueprintLibrary::GetPersistentDataManager(const UObject* WorldContextObject,
	const AGameStateBase* GameState)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		// Cast the GameState to AMHGameState
		if (const AMHGameState* MHGameState = Cast<AMHGameState>(GameState))
		{
			if (MHGameState->PersistentDataManager)
			{
				return MHGameState->PersistentDataManager;
			}
			LL_ERR(World, "UMHNetworkBlueprintLibrary::GetPersistentDataManager : PersistentDataManager was nullptr in GameState");
			return nullptr;
		}
	}
	return nullptr;
}

