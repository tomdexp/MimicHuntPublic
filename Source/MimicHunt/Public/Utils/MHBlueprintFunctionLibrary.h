#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MHBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MIMICHUNT_API UMHBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static bool DoesLevelExist(const FString& LevelName);
};
