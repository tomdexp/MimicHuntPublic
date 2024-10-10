#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MHNetworkBlueprintLibrary.generated.h"

UCLASS()
class MIMICHUNT_API UMHNetworkBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static void ServerTravelToLevel(const UObject* WorldContextObject, const FString& LevelName);
};
