#include "Core/MHGameMode.h"

#include "Utils/LLog.h"

LL_FILE_CVAR(MHGameMode);

AMHGameMode::AMHGameMode()
{
	bUseSeamlessTravel = true;
}

void AMHGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);
	LL_DBG(this, "AMHGameMode::GetSeamlessTravelActorList : bToTransition : {0}", bToTransition);
	for (AActor* Actor : ActorList)
	{
		LL_DBG(this, "AMHGameMode::GetSeamlessTravelActorList : Actor : {0}", Actor->GetName());
	}
}
