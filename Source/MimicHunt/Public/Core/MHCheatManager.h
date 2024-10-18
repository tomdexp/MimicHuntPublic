#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "MHCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class MIMICHUNT_API UMHCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void SummonFakePlayer();

	UFUNCTION(Exec)
	void SummonMimic();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CheatManager")
	TSubclassOf<AActor> PlayerCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CheatManager")
	TSubclassOf<AActor> MimicCharacterClass;
	
	void SpawnActorWithLineTrace(TSubclassOf<AActor> ActorClass);
};
