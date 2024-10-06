#include "Networking/EOSSubsystem.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "UE5Coro.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "IOnlineSubsystemEOS.h"
#include "IEOSSDKManager.h"
#include "Logging/StructuredLog.h"
#include "Online/OnlineSessionNames.h"
#include "eos_sdk.h"
#include "eos_connect.h"
#include "eos_auth.h"
#include "eos_types.h"
#include "eos_common.h"
#include "OnlineSubsystemEOSTypesPublic.h"

#define SESSION_SETTING_JOIN_CODE FName("SETTING_SESSION_JOIN_CODE")


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
    	co_return FCreateSessionResult{ NAME_None, false , 0};
    }
	UE_LOG_ONLINE(Log, TEXT("Session interface is valid!"));

	UE_LOG_ONLINE(Log, TEXT("Creating session settings..."));
    LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
    LastSessionSettings->NumPrivateConnections = 0;
    LastSessionSettings->NumPublicConnections = CreateSessionRequest.NumPublicConnections;
    LastSessionSettings->bAllowInvites = false;
    LastSessionSettings->bAllowJoinInProgress = true;
    LastSessionSettings->bAllowJoinViaPresence = false;
    LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
    LastSessionSettings->bIsDedicated = false;
    LastSessionSettings->bUsesPresence = false;
    LastSessionSettings->bIsLANMatch = CreateSessionRequest.bIsLANMatch;
    LastSessionSettings->bShouldAdvertise = true;

    LastSessionSettings->Set(FName("SETTING_MAPNAME"), FString("Your Level Name"), EOnlineDataAdvertisementType::ViaOnlineService);

	// Generate a 4-digit random number for the session ID
	const int32 JoinCode = FMath::RandRange(1000, 9999);
	LastSessionSettings->Set(SESSION_SETTING_JOIN_CODE, JoinCode, EOnlineDataAdvertisementType::ViaOnlineService);
	CurrentLobbyJoinCode = JoinCode;
	
	UE_LOG_ONLINE(Log, TEXT("Session settings created!"));

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!LocalPlayer)
	{
		UE_LOG_ONLINE(Log, TEXT("Local player is not valid! Cannot create a session."));
		co_return FCreateSessionResult{ NAME_None, false , 0};
	}
	UE_LOG_ONLINE(Log, TEXT("Local player is valid for session creation !"));

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
        co_return FCreateSessionResult{ NAME_None, false, 0};
    }

    // Await the event (suspends the coroutine until the event is triggered)
    co_await AwaitableEvent;

    // Clean up the delegate
    SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(DelegateHandle);

    // Return the result
	UE_LOG_ONLINE(Log, TEXT("Session created successfully with the join code %d!"), CurrentLobbyJoinCode);
    co_return FCreateSessionResult{ SessionName, bWasSuccessful, CurrentLobbyJoinCode};
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

FVoidCoroutine UEOSSubsystem::K2_Login(FLatentActionInfo LatentInfo, FLoginResult& OutResult)
{
	OutResult = co_await Login();
}

UE5Coro::TCoroutine<FLoginResult> UEOSSubsystem::Login()
{
	UE_LOG_ONLINE(Log, TEXT("Starting Login process"));

	IOnlineSubsystemEOS* EOSSubsystem = static_cast<IOnlineSubsystemEOS*>(Online::GetSubsystem(
		GetWorld(), EOS_SUBSYSTEM));
	if (!EOSSubsystem)
	{
		UE_LOG_ONLINE(Log, TEXT("Failed to get EOS Online Subsystem!"));
		co_return FLoginResult(false, FString("Failed to get EOS Online Subsystem!"));
	}
	UE_LOG_ONLINE(Log, TEXT("Successfully retrieved EOS Online Subsystem"));

	EOS_HPlatform PlatformHandle = *EOSSubsystem->GetEOSPlatformHandle();
	if (!PlatformHandle)
	{
		UE_LOG_ONLINE(Log, TEXT("Failed to get EOS Platform Handle!"));
		co_return FLoginResult(false, FString("Failed to get EOS Platform Handle!"));
	}
	UE_LOG_ONLINE(Log, TEXT("Successfully retrieved EOS Platform Handle"));

	EOS_HConnect ConnectHandle = EOS_Platform_GetConnectInterface(PlatformHandle);
	if (!ConnectHandle)
	{
		UE_LOG_ONLINE(Log, TEXT("Failed to get EOS Connect Handle!"));
		co_return FLoginResult(false, FString("Failed to get EOS Connect Handle!"));
	}
	UE_LOG_ONLINE(Log, TEXT("Successfully retrieved EOS Connect Handle"));

	IOnlineIdentityPtr Identity = Online::GetIdentityInterface(GetWorld());
	if (!Identity.IsValid())
	{
		UE_LOG_ONLINE(Log, TEXT("Failed to get Identity Interface!"));
		co_return FLoginResult(false, FString("Failed to get Identity Interface!"));
	}
	UE_LOG_ONLINE(Log, TEXT("Successfully retrieved Identity Interface"));

	FOnlineAccountCredentials Credentials;
	Credentials.Type = "deviceid";
	Identity->Login(0, Credentials);
	co_return FLoginResult(true, FString(), FString());

	/*FUniqueNetIdPtr NetId = Identity->GetUniquePlayerId(0);
	if (NetId.IsValid() && Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		// Already logged in, return success
		UE_LOG_ONLINE(Log, TEXT("Already logged in!"));
		co_return FLoginResult(true, FString(), NetId->ToString());
	}
	UE_LOG_ONLINE(Log, TEXT("Not logged in, proceeding with Device ID creation"));

	// Variables to capture the result
	bool bWasSuccessful = false;
	FString ErrorMessage;

	// --- Step 1: Create the Device ID if it doesn't exist ---
	{
		UE_LOG_ONLINE(Log, TEXT("Creating Device ID"));

		// Create the options for creating a Device ID
		EOS_Connect_CreateDeviceIdOptions CreateDeviceIdOptions = {};
		CreateDeviceIdOptions.ApiVersion = EOS_CONNECT_CREATEDEVICEID_API_LATEST;

		// Get the PC name as the Device Model
		CreateDeviceIdOptions.DeviceModel = reinterpret_cast<const char*>(FPlatformProcess::ComputerName());

		// Variables to capture the result
		EOS_EResult CreateDeviceIdResultCode = EOS_EResult::EOS_ServiceFailure;
		UE5Coro::FAwaitableEvent CreateDeviceIdEvent;

		// Context for the callback
		struct FCreateDeviceIdContext
		{
			UE5Coro::FAwaitableEvent* Event;
			EOS_EResult* ResultCode;
		};

		FCreateDeviceIdContext CreateDeviceIdContext = {&CreateDeviceIdEvent, &CreateDeviceIdResultCode};

		// Call the EOS SDK function
		EOS_Connect_CreateDeviceId(ConnectHandle, &CreateDeviceIdOptions, &CreateDeviceIdContext,
		                           [](const EOS_Connect_CreateDeviceIdCallbackInfo* Data)
		                           {
			                           // Retrieve the context
			                           FCreateDeviceIdContext* Context = static_cast<FCreateDeviceIdContext*>(Data->
				                           ClientData);

			                           // Capture the result code
			                           *(Context->ResultCode) = Data->ResultCode;

			                           // Trigger the event
			                           Context->Event->Trigger();
		                           });

		// Await the event
		co_await CreateDeviceIdEvent;

		// Check the result
		if (CreateDeviceIdResultCode != EOS_EResult::EOS_Success && CreateDeviceIdResultCode !=
			EOS_EResult::EOS_DuplicateNotAllowed)
		{
			UE_LOG_ONLINE(Log, TEXT("Failed to create Device ID. Error: %s"),
			              ANSI_TO_TCHAR(EOS_EResult_ToString(CreateDeviceIdResultCode)));
			ErrorMessage = FString::Printf(
				TEXT("Failed to create Device ID. Error: %s"),
				ANSI_TO_TCHAR(EOS_EResult_ToString(CreateDeviceIdResultCode)));
			co_return FLoginResult(false, ErrorMessage);
		}
		UE_LOG_ONLINE(Log, TEXT("Device ID created or already exists"));
	}

	// --- Step 2: Login with Device ID ---
	{
		UE_LOG_ONLINE(Log, TEXT("Logging in with Device ID"));

		EOS_Connect_Credentials Credentials = {};
		Credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
		Credentials.Token = nullptr;
		Credentials.Type = EOS_EExternalCredentialType::EOS_ECT_DEVICEID_ACCESS_TOKEN;

		EOS_Connect_UserLoginInfo UserLoginInfo = {};
		UserLoginInfo.ApiVersion = EOS_CONNECT_USERLOGININFO_API_LATEST;
		// TODO : Set an appropriate display name
		// Maybe we should let the player enter a name ?
		UserLoginInfo.DisplayName = "Player";

		EOS_Connect_LoginOptions LoginOptions = {};
		LoginOptions.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
		LoginOptions.Credentials = &Credentials;
		LoginOptions.UserLoginInfo = &UserLoginInfo;

		// Variables to capture the result
		EOS_EResult LoginResultCode = EOS_EResult::EOS_ServiceFailure;
		EOS_ProductUserId LocalUserId = nullptr;
		UE5Coro::FAwaitableEvent LoginEvent;

		// Context for the callback
		struct FLoginContext
		{
			UE5Coro::FAwaitableEvent* Event;
			EOS_EResult* ResultCode;
			EOS_ProductUserId* LocalUserId;
		};

		FLoginContext LoginContext = {&LoginEvent, &LoginResultCode, &LocalUserId};

		// Call the EOS SDK function
		EOS_Connect_Login(ConnectHandle, &LoginOptions, &LoginContext, [](const EOS_Connect_LoginCallbackInfo* Data)
		{
			// Retrieve the context
			FLoginContext* Context = static_cast<FLoginContext*>(Data->ClientData);

			// Capture the result code and LocalUserId
			*(Context->ResultCode) = Data->ResultCode;
			*(Context->LocalUserId) = Data->LocalUserId;

			// Trigger the event
			Context->Event->Trigger();
		});

		// Await the event
		co_await LoginEvent;

		// Check the result
		if (LoginResultCode == EOS_EResult::EOS_Success)
		{
			bWasSuccessful = true;
			UE_LOG_ONLINE(Log, TEXT("Successfully logged in with Device ID"));

			EOS_Bool ResultProductUserId = EOS_ProductUserId_IsValid(LocalUserId);
			if (ResultProductUserId == EOS_TRUE)
			{
				UE_LOG_ONLINE(Log, TEXT("ProductUserId is valid"));
			}
			else
			{
				UE_LOG_ONLINE(Log, TEXT("ProductUserId is not valid"));
			}

			// Declare buffer length variable, but initially pass it as 0 to get the required size
			int32_t BufferLength = 0;

			// First, pass a nullptr to get the required buffer size
			EOS_EResult Result = EOS_ProductUserId_ToString(LocalUserId, nullptr, &BufferLength);

			if (Result == EOS_EResult::EOS_LimitExceeded && BufferLength > 0)
			{
				// Allocate the buffer with the required length
				char* ProductUserIdString = new char[BufferLength];

				// Now call the function again with the correctly sized buffer
				Result = EOS_ProductUserId_ToString(LocalUserId, ProductUserIdString, &BufferLength);

				if (Result == EOS_EResult::EOS_Success)
				{
					FString UserIdString(ProductUserIdString);
					UE_LOG_ONLINE(Log, TEXT("Successfully converted ProductUserId to string: %s"), *UserIdString);

					// Use the EOS-specific Identity interface to get the player's UniqueNetId
					FUniqueNetIdPtr NetIdPtr = Identity->GetUniquePlayerId(0);  // 0 = LocalUserNum

					if (NetIdPtr.IsValid())
					{

						// Set the Product User ID for the local player
						FUniqueNetIdRepl NetIdRepl = FUniqueNetIdRepl(NetIdPtr);
						auto LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
        
						if (LocalPlayer)
						{
							LocalPlayer->SetCachedUniqueNetId(NetIdRepl);
							UE_LOG_ONLINE(Log, TEXT("Local player's UniqueNetId set to: %s"), *NetIdPtr->ToString());

							// Check if the UniqueNetId is valid
							if (LocalPlayer->GetCachedUniqueNetId().IsValid())
							{
								UE_LOG_ONLINE(Log, TEXT("Local player's UniqueNetId is valid for session creation!"));
							}
							else
							{
								UE_LOG_ONLINE(Log, TEXT("Local player's UniqueNetId is not valid!"));
							}
						}
						else
						{
							UE_LOG_ONLINE(Log, TEXT("Failed to retrieve LocalPlayer!"));
						}
					}
					else
					{
						UE_LOG_ONLINE(Log, TEXT("Failed to retrieve UniqueNetId from Identity interface"));
					}
				}
				else
				{
					UE_LOG_ONLINE(Log, TEXT("Failed to convert ProductUserId to string. Error: %s"),
					              ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
					co_return FLoginResult(false, FString("Failed to convert ProductUserId to string"));
				}

				// Free the dynamically allocated buffer
				delete[] ProductUserIdString;
			}
			else
			{
				UE_LOG_ONLINE(Log, TEXT("Failed to retrieve required buffer size or invalid ProductUserId. Error: %s"),
				              ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));
				co_return FLoginResult(
					false, FString("Failed to retrieve required buffer size or invalid ProductUserId"));
			}
		}
		else
		{
			bWasSuccessful = false;
			UE_LOG_ONLINE(Log, TEXT("Failed to login with Device ID. Error: %s"),
			              ANSI_TO_TCHAR(EOS_EResult_ToString(LoginResultCode)));
			ErrorMessage = FString::Printf(
				TEXT("Failed to login with Device ID. Error: %s"),
				ANSI_TO_TCHAR(EOS_EResult_ToString(LoginResultCode)));
			co_return FLoginResult(bWasSuccessful, ErrorMessage, FString());
		}
	}*/
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

FVoidCoroutine UEOSSubsystem::K2_JoinSessionWithCode(int32 JoinCode, FLatentActionInfo LatentInfo,
	FJoinSessionResult& OutResult)
{
	OutResult = co_await JoinSessionWithCode(JoinCode);
}

UE5Coro::TCoroutine<FJoinSessionResult> UEOSSubsystem::JoinSessionWithCode(int32 JoinCode)
{
	UE_LOG_ONLINE(Log, TEXT("Trying to join a session with the join code %d..."), JoinCode);
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
    if (!SessionInterface.IsValid())
    {
		UE_LOG_ONLINE(Log, TEXT("Session interface is not valid! Cannot find sessions."));
    	co_return FJoinSessionResult(false);
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
    LastSessionSearch->QuerySettings.Set(SESSION_SETTING_JOIN_CODE, JoinCode, EOnlineComparisonOp::Equals);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
    {
        // Clean up the delegate
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandle);

    	UE_LOG_ONLINE(Log, TEXT("Failed to find sessions!"));
        co_return FJoinSessionResult(false, FString("Failed to find sessions"));
    }

    // Await the event (suspends the coroutine until the event is triggered)
    co_await AwaitableEvent;

    // Clean up the delegate
    SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandle);

    // Return the result
	if (!bWasSuccessful)
	{
		UE_LOG_ONLINE(Log, TEXT("Failed to find sessions with the join code %d"), JoinCode);
		co_return FJoinSessionResult(false, FString("Failed to find sessions with the join code"));
	}
	if (SearchResults.Num() == 0)
	{
		UE_LOG_ONLINE(Log, TEXT("No sessions found with the join code %d"), JoinCode);
		co_return FJoinSessionResult(false, FString("No sessions found with the join code"));
	}
	if (SearchResults.Num() > 1)
	{
		UE_LOG_ONLINE(Log, TEXT("Multiple sessions found with the join code %d"), JoinCode);
		co_return FJoinSessionResult(false, FString("Multiple sessions found with the join code"));
	}
	if (SearchResults.Num() == 1)
	{
		UE_LOG_ONLINE(Log, TEXT("Found session with join code %d successfully"), JoinCode);
		JoinGameSession(SearchResults[0]);
	}
	UE_LOG_ONLINE(Log, TEXT("Found session with join code %d successfully"), JoinCode);
    co_return FJoinSessionResult(bWasSuccessful);
}

FIsValidJoinCodeFormatResult UEOSSubsystem::IsValidJoinCodeFormat(const FString& JoinCode)
{
	if (!JoinCode.IsNumeric())
	{
		return FIsValidJoinCodeFormatResult(false, 0);
	}
	int32 JoinCodeInt = FCString::Atoi(*JoinCode);
	// Check if the join code is a 4-digit number
	if (JoinCodeInt < 1000 || JoinCodeInt > 9999)
	{
		return FIsValidJoinCodeFormatResult(false, 0);
	}
	return FIsValidJoinCodeFormatResult(true, JoinCodeInt);
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
	UE_LOG_ONLINE(Log, TEXT("Trying to travel to the current session..."));
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
