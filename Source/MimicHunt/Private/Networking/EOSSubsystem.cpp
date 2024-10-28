#include "MimicHunt/Public/Networking/EOSSubsystem.h"

FString UEOSSubsystem::GetCurrentLobbyJoinCode()
{
	if (bHasJoinCode)
	{
		return CurrentLobbyJoinCode;
	}
	UE_LOG(LogTemp, Error, TEXT("No join code available, but it was requested. Returning empty string."));
	return FString();
}

FString UEOSSubsystem::CreateLobbyJoinCode()
{
	// A join code is a 4-digit number
	CurrentLobbyJoinCode = FString::FromInt(FMath::RandRange(1000, 9999));
	bHasJoinCode = true;
	return CurrentLobbyJoinCode;
}

void UEOSSubsystem::ClearJoinCode()
{
	bHasJoinCode = false;
	CurrentLobbyJoinCode.Empty();
	
}
