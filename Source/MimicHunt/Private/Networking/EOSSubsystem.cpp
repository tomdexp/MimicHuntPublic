#include "Networking/EOSSubsystem.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "UE5Coro.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Logging/StructuredLog.h"
#include "Online/OnlineSessionNames.h"


UEOSSubsystem::UEOSSubsystem() : JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionCompleted))
{
}

FVoidCoroutine UEOSSubsystem::K2_CreateSession(FCreateSessionRequest CreateSessionRequest,
	FLatentActionInfo LatentInfo, FCreateSessionResult& OutResult)
{
	OutResult = co_await CreateSession(CreateSessionRequest);
}

UE5Coro::TCoroutine<FCreateSessionResult> UEOSSubsystem::CreateSession(FCreateSessionRequest CreateSessionRequest)
{
	UE_LOG_ONLINE(Log, TEXT("Trying to create a session with the EOS subsystem..."));
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
    if (!SessionInterface.IsValid())
    {
		UE_LOG_ONLINE(Log, TEXT("Session interface is not valid! Cannot create a session."));
    	co_return FCreateSessionResult{ NAME_None, false };
    }

    LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
    LastSessionSettings->NumPrivateConnections = 0;
    LastSessionSettings->NumPublicConnections = CreateSessionRequest.NumPublicConnections;
    LastSessionSettings->bAllowInvites = true;
    LastSessionSettings->bAllowJoinInProgress = true;
    LastSessionSettings->bAllowJoinViaPresence = true;
    LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = true;
    LastSessionSettings->bIsDedicated = false;
    LastSessionSettings->bUsesPresence = true;
    LastSessionSettings->bIsLANMatch = CreateSessionRequest.bIsLANMatch;
    LastSessionSettings->bShouldAdvertise = true;

    LastSessionSettings->Set(FName("SETTING_MAPNAME"), FString("Your Level Name"), EOnlineDataAdvertisementType::ViaOnlineService);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

    // Variables to capture the delegate results
    FName SessionName;
    bool bWasSuccessful = false;

    // Create an awaitable event
    UE5Coro::FAwaitableEvent AwaitableEvent;

    // Bind the delegate
    FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
    OnCreateSessionCompleteDelegate.BindLambda([&](FName InSessionName, bool bSuccess)
    {
        // Capture the results
        SessionName = InSessionName;
        bWasSuccessful = bSuccess;
        // Trigger the event to resume the coroutine
        AwaitableEvent.Trigger();
    });

    // Add the delegate
    FDelegateHandle DelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

    // Start the session creation
    if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
    {
        // Clean up the delegate
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(DelegateHandle);
    	UE_LOG_ONLINE(Log, TEXT("Failed to create the session!"));
        co_return FCreateSessionResult{ NAME_None, false };
    }

    // Await the event (suspends the coroutine until the event is triggered)
    co_await AwaitableEvent;

    // Clean up the delegate
    SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(DelegateHandle);

    // Return the result
	UE_LOG_ONLINE(Log, TEXT("Session created successfully!"));
    co_return FCreateSessionResult{ SessionName, bWasSuccessful };
}

FVoidCoroutine UEOSSubsystem::K2_UpdateSession(FLatentActionInfo LatentInfo, FUpdateSessionResult& OutResult)
{
	OutResult = co_await UpdateSession();
}

UE5Coro::TCoroutine<FUpdateSessionResult> UEOSSubsystem::UpdateSession()
{
	UE_LOG_ONLINE(Log, TEXT("Trying to update the session with the EOS subsystem..."));
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		UE_LOG_ONLINE(Log, TEXT("Session interface is not valid! Cannot update the session."));
		co_return FUpdateSessionResult(NAME_None, false);
	}

	TSharedPtr<FOnlineSessionSettings> UpdatedSessionSettings = MakeShareable(new FOnlineSessionSettings(*LastSessionSettings));
	UpdatedSessionSettings->Set(FName("SETTING_MAPNAME"), FString("Updated Level Name"), EOnlineDataAdvertisementType::ViaOnlineService);

	// Variables to capture the delegate results
	FName SessionName = NAME_None;
	bool bWasSuccessful = false;

	// Create an awaitable event
	UE5Coro::FAwaitableEvent AwaitableEvent;

	// Bind the delegate
	FOnUpdateSessionCompleteDelegate OnUpdateSessionCompleteDelegate;
	OnUpdateSessionCompleteDelegate.BindLambda([&](FName InSessionName, bool bSuccess)
	{
		// Capture the results
		SessionName = InSessionName;
		bWasSuccessful = bSuccess;
		// Trigger the event to resume the coroutine
		AwaitableEvent.Trigger();
	});

	// Add the delegate
	FDelegateHandle DelegateHandle = SessionInterface->AddOnUpdateSessionCompleteDelegate_Handle(OnUpdateSessionCompleteDelegate);

	// Attempt to update the session
	if (!SessionInterface->UpdateSession(NAME_GameSession, *UpdatedSessionSettings))
	{
		// Clean up the delegate
		SessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(DelegateHandle);
		UE_LOG_ONLINE(Log, TEXT("Failed to update the session!"));
		co_return FUpdateSessionResult(NAME_None, false);
	}
	
	LastSessionSettings = UpdatedSessionSettings;

	// Await the event (suspends the coroutine until the event is triggered)
	co_await AwaitableEvent;

	// Clean up the delegate
	SessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(DelegateHandle);

	// Return the result
	UE_LOG_ONLINE(Log, TEXT("Session updated successfully!"));
	co_return FUpdateSessionResult(SessionName, bWasSuccessful);
}

FVoidCoroutine UEOSSubsystem::K2_StartSession(FLatentActionInfo LatentInfo, FStartSessionResult& OutResult)
{
	OutResult = co_await StartSession();
}

UE5Coro::TCoroutine<FStartSessionResult> UEOSSubsystem::StartSession()
{
	UE_LOG_ONLINE(Log, TEXT("Trying to start the session with the EOS subsystem..."));
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		UE_LOG_ONLINE(Log, TEXT("Session interface is not valid! Cannot start the session."));
		co_return FStartSessionResult(NAME_None, false);
	}

	// Variables to capture the delegate results
	FName SessionName = NAME_None;
	bool bWasSuccessful = false;

	// Create an awaitable event
	UE5Coro::FAwaitableEvent AwaitableEvent;

	// Bind the delegate
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	OnStartSessionCompleteDelegate.BindLambda([&](FName InSessionName, bool bSuccess)
	{
		// Capture the results
		SessionName = InSessionName;
		bWasSuccessful = bSuccess;
		// Trigger the event to resume the coroutine
		AwaitableEvent.Trigger();
	});

	// Add the delegate
	FDelegateHandle DelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

	// Start the session
	if (!SessionInterface->StartSession(NAME_GameSession))
	{
		// Clean up the delegate
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(DelegateHandle);
		UE_LOG_ONLINE(Log, TEXT("Failed to start the session!"));
		co_return FStartSessionResult(NAME_None, false);
	}

	// Await the event (suspends the coroutine until the event is triggered)
	co_await AwaitableEvent;

	// Clean up the delegate
	SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(DelegateHandle);

	// Return the result
	UE_LOG_ONLINE(Log, TEXT("Session started successfully!"));
	co_return FStartSessionResult(SessionName, bWasSuccessful);
}

FVoidCoroutine UEOSSubsystem::K2_DestroySession(FLatentActionInfo LatentInfo, FDestroySessionResult& OutResult)
{
	OutResult = co_await DestroySession();
}

UE5Coro::TCoroutine<FDestroySessionResult> UEOSSubsystem::DestroySession()
{
	UE_LOG_ONLINE(Log, TEXT("Trying to destroy the session with the EOS subsystem..."));
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		UE_LOG_ONLINE(Log, TEXT("Session interface is not valid! Cannot destroy the session."));
		co_return FDestroySessionResult(NAME_None, false);
	}

	FName SessionName = NAME_GameSession;
	bool bWasSuccessful = false;

	// Create an awaitable event
	UE5Coro::FAwaitableEvent AwaitableEvent;

	// Bind the delegate
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	OnDestroySessionCompleteDelegate.BindLambda([&](FName InSessionName, bool bSuccess)
	{
		// Capture the results
		SessionName = InSessionName;
		bWasSuccessful = bSuccess;
		// Trigger the event to resume the coroutine
		AwaitableEvent.Trigger();
	});

	// Add the delegate
	FDelegateHandle DelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

	// Attempt to destroy the session
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		// Clean up the delegate
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DelegateHandle);
		UE_LOG_ONLINE(Log, TEXT("Failed to destroy the session!"));
		co_return FDestroySessionResult(NAME_None, false);
	}

	// Await the event (suspends the coroutine until the event is triggered)
	co_await AwaitableEvent;

	// Clean up the delegate
	SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DelegateHandle);

	// Return the result
	UE_LOG_ONLINE(Log, TEXT("Session destroyed successfully!"));
	co_return FDestroySessionResult(SessionName, bWasSuccessful);
}

FVoidCoroutine UEOSSubsystem::K2_FindSessions(FFindSessionRequest FindSessionRequest, FLatentActionInfo LatentInfo,
	FFindSessionsResult& OutResult)
{
	OutResult = co_await FindSessions(FindSessionRequest);
}

UE5Coro::TCoroutine<FFindSessionsResult> UEOSSubsystem::FindSessions(FFindSessionRequest FindSessionRequest)
{
	UE_LOG_ONLINE(Log, TEXT("Trying to find sessions with the EOS subsystem..."));
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
    if (!SessionInterface.IsValid())
    {
		UE_LOG_ONLINE(Log, TEXT("Session interface is not valid! Cannot find sessions."));
    	co_return FFindSessionsResult(false);
    }

    // Variables to capture the delegate results
    TArray<FOnlineSessionSearchResult> SearchResults;
    bool bWasSuccessful = false;
	
    // Create an awaitable event
    UE5Coro::FAwaitableEvent AwaitableEvent;

    // Bind the delegate
    FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
    OnFindSessionsCompleteDelegate.BindLambda([&](bool bSuccess)
    {
        // Capture the results
        bWasSuccessful = bSuccess;
        if (LastSessionSearch.IsValid())
        {
            SearchResults = LastSessionSearch->SearchResults;
        }
        // Trigger the event to resume the coroutine
        AwaitableEvent.Trigger();
    });

    // Add the delegate
    FDelegateHandle DelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

    // Set up the session search
    LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
    LastSessionSearch->MaxSearchResults = FindSessionRequest.MaxSearchResults;
    LastSessionSearch->bIsLanQuery = FindSessionRequest.bIsLANQuery;

    LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
    {
        // Clean up the delegate
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandle);

    	UE_LOG_ONLINE(Log, TEXT("Failed to find sessions!"));
        co_return FFindSessionsResult(false);
    }

    // Await the event (suspends the coroutine until the event is triggered)
    co_await AwaitableEvent;

    // Clean up the delegate
    SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandle);

    // Return the result
	UE_LOG_ONLINE(Log, TEXT("Found %d sessions successfully"), SearchResults.Num());
    co_return FFindSessionsResult(bWasSuccessful);
}

void UEOSSubsystem::JoinGameSession(const FOnlineSessionSearchResult& SessionResult)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnJoinGameSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle =
		sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		OnJoinGameSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UEOSSubsystem::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	OnJoinGameSessionCompleteEvent.Broadcast(Result);
}

bool UEOSSubsystem::TryTravelToCurrentSession()
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		return false;
	}

	FString connectString;
	if (!sessionInterface->GetResolvedConnectString(NAME_GameSession, connectString))
	{
		return false;
	}

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	playerController->ClientTravel(connectString, TRAVEL_Absolute);
	return true;
}
