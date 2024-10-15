// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Blueprint.h"
#include "SubobjectDataSubsystem.h"
#include "MimicEditorBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MIMICHUNT_API UMimicEditorBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	public:
	UFUNCTION(BlueprintCallable, Category = "Mimic")
	static void ComputeMimicBlueprint(UBlueprint* Blueprint);
};
