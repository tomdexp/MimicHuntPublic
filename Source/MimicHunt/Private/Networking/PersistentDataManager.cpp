#include "Networking/PersistentDataManager.h"

#include "Net/UnrealNetwork.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(PersistentDataManager);

APersistentDataManager::APersistentDataManager()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
}

void APersistentDataManager::BeginPlay()
{
	Super::BeginPlay();
	LL_DBG(this, "APersistentDataManager::BeginPlay");

	// if(HasAuthority())
	// {
	// 	// Increase the money every 2 seconds on the server
	// 	FTimerHandle TimerHandleIncreaseMoney;
	// 	GetWorldTimerManager().SetTimer(TimerHandleIncreaseMoney, [this]()
	// 	{
	// 		PersistentData.LobbyMoney += 1;
	// 	}, 2.f, true);
	// }
}

void APersistentDataManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APersistentDataManager, PersistentData);
}


