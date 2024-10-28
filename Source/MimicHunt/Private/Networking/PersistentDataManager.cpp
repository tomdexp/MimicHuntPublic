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

void APersistentDataManager::OnRep_PersistentData()
{
	LL_DBG(this, "APersistentDataManager::OnRep_PersistentData LobbyMoney: {0}", PersistentData.LobbyMoney);
	OnPersistentDataChanged.Broadcast(PersistentData);
}

int32 APersistentDataManager::GetLobbyMoney() const
{
	return PersistentData.LobbyMoney;
}

void APersistentDataManager::AddLobbyMoney(int32 Amount)
{
	if (!HasAuthority())
	{
		LL_WRN(this, "APersistentDataManager::AddLobbyMoney called on client but can only be called on server");
		return;
	}
	LL_DBG(this, "APersistentDataManager::AddLobbyMoney {0}", Amount);
	PersistentData.LobbyMoney += Amount;
	OnRep_PersistentData();
}

void APersistentDataManager::RemoveLobbyMoney(int32 Amount)
{
	if (!HasAuthority())
	{
		LL_WRN(this, "APersistentDataManager::RemoveLobbyMoney called on client but can only be called on server");
		return;
	}
	LL_DBG(this, "APersistentDataManager::RemoveLobbyMoney {0}", Amount);
	PersistentData.LobbyMoney -= Amount;
	OnRep_PersistentData();
}

void APersistentDataManager::SetLobbyMoney(int32 Amount)
{
	if (!HasAuthority())
	{
	 	LL_WRN(this, "APersistentDataManager::SetLobbyMoney called on client but can only be called on server");
	 	return;
	}
	LL_DBG(this, "APersistentDataManager::SetLobbyMoney {0}", Amount);
	PersistentData.LobbyMoney = Amount;
	OnRep_PersistentData();
}


