#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EOSSubsystem.generated.h"


UCLASS()
class MIMICHUNT_API UEOSSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetCurrentLobbyJoinCode();

	UFUNCTION(BlueprintCallable)
	FString CreateLobbyJoinCode();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasJoinCode() const { return bHasJoinCode; }
	
private:
	FString CurrentLobbyJoinCode;
	bool bHasJoinCode;
};
