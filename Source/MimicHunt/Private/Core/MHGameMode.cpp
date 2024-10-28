#include "Core/MHGameMode.h"

#include "EngineUtils.h"
#include "MHPlayerCharacter.h"
#include "Core/MHGameState.h"
#include "Core/MHPlayerController.h"
#include "GameFramework/SpectatorPawn.h"
#include "Utils/LLog.h"
#include "Networking/PersistentDataManager.h"

class AMHGameState;

LL_FILE_CVAR(MHGameMode);

AMHGameMode::AMHGameMode()
{
	bUseSeamlessTravel = true;
}

void AMHGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);
	
	ActorList.Add(PersistentDataManager);
}

void AMHGameMode::InitGameState()
{
	Super::InitGameState();

	// Check if the PersistentDataManager is spawned and assign it in the game state
	for (APersistentDataManager* DataManager : TActorRange<APersistentDataManager>(GetWorld()))
	{
		if (DataManager)
		{
			PersistentDataManager = DataManager; // Reassign the PersistentDataManager
			LL_DBG(this, "AMHGameMode::InitGameState : Found PersistentDataManager, reassigning it");
			
			if(AMHGameState* GS = GetGameState<AMHGameState>())
			{
				GS->PersistentDataManager = DataManager; // Cache it in GameState so it's easily accessible client-side
				LL_DBG(this, "AMHGameMode::InitGameState : Cached PersistentDataManager in GameState");
			}
			return;
		}
		LL_ERR(this, "AMHGameMode::InitGameState : Found PersistentDataManager but it was nullptr");
		return;
	}
	LL_DBG(this, "AMHGameMode::InitGameState : PersistentDataManager was not found (maybe it not spawned yet which is normal then)");
}

void AMHGameMode::PlayerCharacterDied(AController* Controller)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, Controller->GetPawn()->GetActorTransform(), SpawnParameters);

	Controller->UnPossess();
	Controller->Possess(SpectatorPawn);

	FTimerHandle RespawnTimerHandle;

	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &AMHGameMode::RespawnPlayerCharacter,Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, 5, false);

	AMHPlayerController* PC = Cast<AMHPlayerController>(Controller);
	if (PC)
	{
		// TODO : Set UI Callback here
		//PC->SetRespawnCountdown(RespawnDelay);
	}
}

bool AMHGameMode::AllowCheats(APlayerController* P)
{
	return true;
}

void AMHGameMode::RespawnPlayerCharacter(AController* Controller)
{
	if (Controller->IsPlayerController())
	{
		// Respawn player
		AActor* PlayerStart = FindPlayerStart(Controller);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AMHPlayerCharacter* PlayerCharacter = GetWorld()->SpawnActor<AMHPlayerCharacter>(PlayerCharacterClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParameters);

		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(PlayerCharacter);
	}
	/*else
	{
		// Respawn AI hero
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AGDHeroCharacter* Hero = GetWorld()->SpawnActor<AGDHeroCharacter>(HeroClass, EnemySpawnPoint->GetActorTransform(), SpawnParameters);
		
		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Hero);
	}*/
}
