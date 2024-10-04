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

	FCreateSessionResult()
		: SessionName(NAME_None), bWasSuccessful(false)
	{}

	FCreateSessionResult(FName Name, bool bCond)
		: SessionName(Name), bWasSuccessful(bCond)
	{}
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

UCLASS()
class MIMICHUNT_API UEOSSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UEOSSubsystem();

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
	FVoidCoroutine K2_CreateSession(FCreateSessionRequest CreateSessionRequest, FLatentActionInfo LatentInfo, FCreateSessionResult& OutResult);
	UE5Coro::TCoroutine<FCreateSessionResult> CreateSession(FCreateSessionRequest CreateSessionRequest);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
	FVoidCoroutine K2_UpdateSession(FLatentActionInfo LatentInfo, FUpdateSessionResult& OutResult);
	UE5Coro::TCoroutine<FUpdateSessionResult> UpdateSession();

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
	FVoidCoroutine K2_StartSession(FLatentActionInfo LatentInfo, FStartSessionResult& OutResult);
	UE5Coro::TCoroutine<FStartSessionResult> StartSession();

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
	FVoidCoroutine K2_DestroySession(FLatentActionInfo LatentInfo, FDestroySessionResult& OutResult);
	UE5Coro::TCoroutine<FDestroySessionResult> DestroySession();

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
	FVoidCoroutine K2_FindSessions(FFindSessionRequest FindSessionRequest, FLatentActionInfo LatentInfo, FFindSessionsResult& OutResult);
	UE5Coro::TCoroutine<FFindSessionsResult> FindSessions(FFindSessionRequest FindSessionRequest);

	
	void JoinGameSession(const FOnlineSessionSearchResult& SessionResult);
	
	FCSOnJoinSessionComplete OnJoinGameSessionCompleteEvent;

protected:
	void OnFindSessionsCompleted(bool Successful);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	bool TryTravelToCurrentSession();

private:
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
};
