#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "PersistentDataManager.generated.h"

USTRUCT(BlueprintType)
struct FPersistentData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 LobbyMoney = 0;
};

/**
 * This class will persist in the online gameplay between levels, it should be reset when the online gameplay ends
 * It is inspired by this documentation : https://wizardcell.com/unreal/persistent-data/#5-getseamlesstravelactorlist
 * It is used with GetSeamlessTravelActorList() in the GameMode
 */
UCLASS()
class MIMICHUNT_API APersistentDataManager : public AInfo
{
	GENERATED_BODY()

public:
	APersistentDataManager();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Persistent Data")
	FPersistentData PersistentData;
};
