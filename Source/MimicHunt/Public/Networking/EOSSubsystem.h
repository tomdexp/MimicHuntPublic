#pragma once

#include "CoreMinimal.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UE5Coro.h"

#include "EOSSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FCSOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);

USTRUCT(BlueprintType)
struct FCreateSessionRequest
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	int32 NumPublicConnections;

	UPROPERTY(BlueprintReadWrite)
	bool bIsLANMatch;

	FCreateSessionRequest()
		: NumPublicConnections(0), bIsLANMatch(false)
	{}

	FCreateSessionRequest(int32 NumConnections, bool bIsLAN)
		: NumPublicConnections(NumConnections), bIsLANMatch(bIsLAN)
	{}
};


USTRUCT(BlueprintType)
struct FCreateSessionResult
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FName SessionName;
	
	UPROPERTY(BlueprintReadWrite)
	bool bWasSuccessful;

	UPROPERTY(BlueprintReadWrite)
	int32 JoinCode;

	FCreateSessionResult()
		: SessionName(NAME_None), bWasSuccessful(false), JoinCode(0)
	{
	}

	FCreateSessionResult(FName Name, bool bCond, int32 JoinCode)
		: SessionName(Name), bWasSuccessful(bCond), JoinCode(JoinCode)
	{
	}
};

USTRUCT(BlueprintType)
struct FStartSessionResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName SessionName;

	UPROPERTY(BlueprintReadWrite)
	bool bWasSuccessful;

	FStartSessionResult()
		: SessionName(NAME_None), bWasSuccessful(false)
	{}

	FStartSessionResult(FName Name, bool bSuccess)
		: SessionName(Name), bWasSuccessful(bSuccess)
	{}
};

USTRUCT(BlueprintType)
struct FUpdateSessionResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName SessionName;

	UPROPERTY(BlueprintReadWrite)
	bool bWasSuccessful;

	FUpdateSessionResult()
		: SessionName(NAME_None), bWasSuccessful(false)
	{}

	FUpdateSessionResult(FName Name, bool bSuccess)
		: SessionName(Name), bWasSuccessful(bSuccess)
	{}
};

USTRUCT(BlueprintType)
struct FDestroySessionResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName SessionName;

	UPROPERTY(BlueprintReadWrite)
	bool bWasSuccessful;

	FDestroySessionResult()
		: SessionName(NAME_None), bWasSuccessful(false)
	{}

	FDestroySessionResult(FName Name, bool bSuccess)
		: SessionName(Name), bWasSuccessful(bSuccess)
	{}
};

USTRUCT(BlueprintType)
struct FFindSessionRequest
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	int32 MaxSearchResults;

	UPROPERTY(BlueprintReadWrite)
	bool bIsLANQuery;

	FFindSessionRequest()
		: MaxSearchResults(0), bIsLANQuery(false)
	{}

	FFindSessionRequest(int32 MaxSearchResults, bool bIsLANQuery)
		: MaxSearchResults(MaxSearchResults), bIsLANQuery(bIsLANQuery)
	{}
};

USTRUCT(BlueprintType)
struct FFindSessionsResult
{
	GENERATED_BODY()
	
    
	UPROPERTY(BlueprintReadOnly)
	bool bWasSuccessful;
    
	FFindSessionsResult()
		: bWasSuccessful(false)
	{}
    
	FFindSessionsResult(bool bSuccess)
		: bWasSuccessful(bSuccess)
	{}
};

USTRUCT(BlueprintType)
struct FLoginResult
{
	GENERATED_BODY()
    
	UPROPERTY(BlueprintReadOnly)
	bool bWasSuccessful;

	UPROPERTY(BlueprintReadOnly)
	FString ErrorMessage;

	UPROPERTY(BlueprintReadOnly)
	FString UserId;

	FLoginResult()
		: bWasSuccessful(false)
	{}

	FLoginResult(bool bSuccess, const FString& InErrorMessage = FString(), const FString& InUserId = FString())
		: bWasSuccessful(bSuccess), ErrorMessage(InErrorMessage), UserId(InUserId)
	{}
};

USTRUCT(BlueprintType)
struct FJoinSessionResult
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	bool bWasSuccessful;

	UPROPERTY(BlueprintReadOnly)
	FString ErrorMessage;

	FJoinSessionResult()
		: bWasSuccessful(false)
	{}

	FJoinSessionResult(bool bSuccess, const FString& InErrorMessage = FString())
		: bWasSuccessful(bSuccess), ErrorMessage(InErrorMessage)
	{}
};

USTRUCT(BlueprintType)
struct FIsValidJoinCodeFormatResult
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsValid;

	UPROPERTY(BlueprintReadOnly)
	int32 JoinCode;

	FIsValidJoinCodeFormatResult()
		: bIsValid(false), JoinCode(0)
	{}

	FIsValidJoinCodeFormatResult(bool bValid, int32 Code)
		: bIsValid(bValid), JoinCode(Code)
	{}
};

UCLASS()
class MIMICHUNT_API UEOSSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UEOSSubsystem();

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo, DisplayName = "Create Session"))
	FVoidCoroutine K2_CreateSession(FCreateSessionRequest CreateSessionRequest, FLatentActionInfo LatentInfo, FCreateSessionResult& OutResult);
	UE5Coro::TCoroutine<FCreateSessionResult> CreateSession(FCreateSessionRequest CreateSessionRequest);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo, DisplayName = "Update Session"))
	FVoidCoroutine K2_UpdateSession(FLatentActionInfo LatentInfo, FUpdateSessionResult& OutResult);
	UE5Coro::TCoroutine<FUpdateSessionResult> UpdateSession();

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo, DisplayName = "Start Session"))
	FVoidCoroutine K2_StartSession(FLatentActionInfo LatentInfo, FStartSessionResult& OutResult);
	UE5Coro::TCoroutine<FStartSessionResult> StartSession();

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo, DisplayName = "Destroy Session"))
	FVoidCoroutine K2_DestroySession(FLatentActionInfo LatentInfo, FDestroySessionResult& OutResult);
	UE5Coro::TCoroutine<FDestroySessionResult> DestroySession();

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo, DisplayName = "Find Sessions"))
	FVoidCoroutine K2_FindSessions(FFindSessionRequest FindSessionRequest, FLatentActionInfo LatentInfo, FFindSessionsResult& OutResult);
	UE5Coro::TCoroutine<FFindSessionsResult> FindSessions(FFindSessionRequest FindSessionRequest);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo, DisplayName = "Login"))
	FVoidCoroutine K2_Login(FLatentActionInfo LatentInfo, FLoginResult& OutResult);
	UE5Coro::TCoroutine<FLoginResult> Login();
	
	void JoinGameSession(const FOnlineSessionSearchResult& SessionResult);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo, DisplayName = "Join Session With Code"))
	FVoidCoroutine K2_JoinSessionWithCode(int32 JoinCode, FLatentActionInfo LatentInfo, FJoinSessionResult& OutResult);
	UE5Coro::TCoroutine<FJoinSessionResult> JoinSessionWithCode(int32 JoinCode);

	UFUNCTION(BlueprintCallable)
	FIsValidJoinCodeFormatResult IsValidJoinCodeFormat(const FString& JoinCode);
	
	FCSOnJoinSessionComplete OnJoinGameSessionCompleteEvent;

protected:
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	bool TryTravelToCurrentSession();

private:
	bool CreateOrRetrieveDeviceId();
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	int32 CurrentLobbyJoinCode;
};
