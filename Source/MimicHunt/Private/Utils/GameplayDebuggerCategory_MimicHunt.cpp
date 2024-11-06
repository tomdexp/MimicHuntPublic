#include "Utils/GameplayDebuggerCategory_MimicHunt.h"

#if WITH_GAMEPLAY_DEBUGGER

#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

FGameplayDebuggerCategory_MimicHunt::FGameplayDebuggerCategory_MimicHunt()
{
	SetDataPackReplication(&DataPack);
}

void FGameplayDebuggerCategory_MimicHunt::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
	if (OwnerPC)
	{
		DataPack.ActorName = OwnerPC->GetPawn()->GetName();
	}
}

void FGameplayDebuggerCategory_MimicHunt::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
{
	if (!DataPack.ActorName.IsEmpty())
	{
		CanvasContext.Printf(TEXT("{yellow}Actor name: {white}%s"), *DataPack.ActorName);
	}
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_MimicHunt::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_MimicHunt());
}

void FGameplayDebuggerCategory_MimicHunt::FRepData::Serialize(FArchive& Ar)
{
	Ar << ActorName;
}

#endif // WITH_GAMEPLAY_DEBUGGER
