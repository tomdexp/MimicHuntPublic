#include "Core/MHGameInstance.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void UMHGameInstance::Init()
{
	Super::Init();
	AutoLogin();
}

void UMHGameInstance::AutoLogin()
{
	// If we are in editor, we auto-login using hard-coded credentials
	// If we are shipping, we used device ID to auto-login (but we can't have two shipping builds on the same device)
#if UE_BUILD_SHIPPING
	
#else
	int32 UserNum = 0;
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		// We are only interested in PIE (Play In Editor) contexts
		if (Context.WorldType == EWorldType::PIE)
		{
			UserNum++;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("UserNum: %d"), UserNum);

	// Get the Online subsystem
	// IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	// IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	// FOnlineAccountCredentials Credentials;
	// Credentials.Type = TEXT("developer");
	// Credentials.Id = TEXT("localhost:6300");
	// Credentials.Token = TEXT("Context_1");
	// Identity->Login(0, Credentials);
#endif
}
