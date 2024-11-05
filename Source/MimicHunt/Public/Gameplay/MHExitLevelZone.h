#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UE5Coro.h"
#include "MHExitLevelZone.generated.h"

class AMHPlayerCharacter;
class UBoxComponent;

/**
 *  A zone that the players has to reach to exit the level
 *  This zone can only activate if all living players are inside it
 */
UCLASS(Blueprintable)
class MIMICHUNT_API AMHExitLevelZone : public AActor
{
	GENERATED_BODY()

public:
	AMHExitLevelZone();

	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxTriggerComponent;

	UPROPERTY(VisibleAnywhere, Category = "ExitLevelZone")
	TArray<TWeakObjectPtr<AMHPlayerCharacter>> PlayersInsideZone;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ExitLevelZone")
	int32 SecondsBeforeExit = 5;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ExitLevelZone")
	int32 NumberOfPlayersInsideZone = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ExitLevelZone")
	int32 NumberOfPlayersAliveInGame = 0;

	UFUNCTION(BlueprintNativeEvent, Category = "ExitLevelZone")
	void OnAllPlayersInsideZone();

	UFUNCTION(BlueprintNativeEvent, Category = "ExitLevelZone")
	void OnExitCancelled();

	UFUNCTION(BlueprintNativeEvent, Category = "ExitLevelZone")
	void OnExitConfirmed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ExitLevelZone")
	bool bIsExiting = false;

	UE5Coro::TCoroutine<> ExitLevelCoroutine();
	
	std::optional<UE5Coro::TCoroutine<>> ExitCoroutine;

};
