#include "Networking/MHNetworkBlueprintLibrary.h"

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
