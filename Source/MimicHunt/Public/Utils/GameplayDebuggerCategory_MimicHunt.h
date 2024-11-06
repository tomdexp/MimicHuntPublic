#pragma once

#if WITH_GAMEPLAY_DEBUGGER

#include "CoreMinimal.h"
#include "GameplayDebuggerCategory.h"

class APlayerController;
class AActor;

class FGameplayDebuggerCategory_MimicHunt : public FGameplayDebuggerCategory
{
public:
	FGameplayDebuggerCategory_MimicHunt();
	void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;
	void DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) override;
    
	static TSharedRef<FGameplayDebuggerCategory> MakeInstance();
    
protected:
	struct FRepData
	{
		// Put all data you want to display here
		FString ActorName;
        
		void Serialize(FArchive& Ar);
	};
    
	FRepData DataPack;
};

#endif // WITH_GAMEPLAY_DEBUGGER
