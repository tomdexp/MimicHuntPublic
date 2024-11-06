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

	UFUNCTION(Exec)
	void AddLobbyMoney(int32 Amount);

	UFUNCTION(Exec)
	void RemoveLobbyMoney(int32 Amount);

	UFUNCTION(Exec)
	void SetLobbyMoney(int32 Amount);

	UFUNCTION(Exec)
	void GetLobbyMoney();

	UFUNCTION(Exec)
	void BirthMimic();

	UFUNCTION(Exec)
	void WakeMimic();

	UFUNCTION(Exec)
	void SleepMimic();

	UFUNCTION(Exec)
	void PossessCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CheatManager")
	TSubclassOf<AActor> PlayerCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CheatManager")
	TSubclassOf<AActor> MimicCharacterClass;
	
	void SpawnActorWithLineTrace(TSubclassOf<AActor> ActorClass);
};
