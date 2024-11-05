#include "Gameplay/MHExitLevelZone.h"

#include "MHPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Core/MHGameState.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(MHExitLevelZone);

AMHExitLevelZone::AMHExitLevelZone()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;

	BoxTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTriggerComponent"));
	BoxTriggerComponent->SetCollisionProfileName("OverlapAll");
	BoxTriggerComponent->UpdateCollisionProfile();
	BoxTriggerComponent->SetGenerateOverlapEvents(true);
	RootComponent = BoxTriggerComponent;

	BoxTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AMHExitLevelZone::OnBeginOverlap);
	BoxTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AMHExitLevelZone::OnEndOverlap);
}

void AMHExitLevelZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(AMHGameState* GameState = GetWorld()->GetGameState<AMHGameState>())
	{
		NumberOfPlayersAliveInGame = GameState->GetAlivePlayerCount();
		if (NumberOfPlayersInsideZone == NumberOfPlayersAliveInGame)
		{
			if (!bIsExiting)
			{
				ExitCoroutine = ExitLevelCoroutine();
			}
		}
		else
		{
			if (bIsExiting && !ExitCoroutine->IsDone())
			{
				LL_DBG(this, "AMHExitLevelZone::Tick : Cancelling exit coroutine");
				ExitCoroutine->Cancel();
				ExitCoroutine.reset();
				OnExitCancelled();
				bIsExiting = false;
			}
		}
	}
}

void AMHExitLevelZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMHPlayerCharacter* Player = Cast<AMHPlayerCharacter>(OtherActor))
	{
		LL_DBG(this, "AMHExitLevelZone::OnBeginOverlap : Player {0} entered the zone", Player->GetPlayerState()->GetPlayerName());
		PlayersInsideZone.Add(Player);
		NumberOfPlayersInsideZone = PlayersInsideZone.Num();
	}
}

void AMHExitLevelZone::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMHPlayerCharacter* Player = Cast<AMHPlayerCharacter>(OtherActor))
	{
		PlayersInsideZone.Remove(Player);
		LL_DBG(this, "AMHExitLevelZone::OnEndOverlap : Player {0} left the zone", Player->GetPlayerState()->GetPlayerName());
		NumberOfPlayersInsideZone = PlayersInsideZone.Num();
	}
}

UE5Coro::TCoroutine<> AMHExitLevelZone::ExitLevelCoroutine()
{
	bIsExiting = true;
	OnAllPlayersInsideZone();
	LL_DBG(this, "AMHExitLevelZone::ExitLevelCoroutine : Exiting level in {0} seconds...", SecondsBeforeExit);
	co_await UE5Coro::Latent::Seconds(SecondsBeforeExit);
	LL_DBG(this, "AMHExitLevelZone::ExitLevelCoroutine : Exiting level");
	OnExitConfirmed();
	bIsExiting = false;
	co_return;
}

void AMHExitLevelZone::OnAllPlayersInsideZone_Implementation()
{
	// Don't use it here, it's meant to be used by the blueprint child class
}

void AMHExitLevelZone::OnExitCancelled_Implementation()
{
	// Don't use it here, it's meant to be used by the blueprint child class
}

void AMHExitLevelZone::OnExitConfirmed_Implementation()
{
	// Don't use it here, it's meant to be used by the blueprint child class
}