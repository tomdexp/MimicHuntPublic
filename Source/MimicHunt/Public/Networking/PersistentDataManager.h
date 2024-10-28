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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPersistentDataChanged, const FPersistentData&, PersistentData);

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
	

	UPROPERTY(ReplicatedUsing=OnRep_PersistentData, BlueprintReadOnly, Category = "Persistent Data")
	FPersistentData PersistentData;

	UFUNCTION()
	virtual void OnRep_PersistentData();

	UPROPERTY(BlueprintAssignable, Category = "Persistent Data")
	FOnPersistentDataChanged OnPersistentDataChanged;

	UFUNCTION(BlueprintCallable, Category = "Persistent Data|Money")
	int32 GetLobbyMoney() const;
	
	UFUNCTION(BlueprintCallable, Category = "Persistent Data|Money")
	void AddLobbyMoney(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Persistent Data|Money")
	void RemoveLobbyMoney(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Persistent Data|Money")
	void SetLobbyMoney(int32 Amount);
};
