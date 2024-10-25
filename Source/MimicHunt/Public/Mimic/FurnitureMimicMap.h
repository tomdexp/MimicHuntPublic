// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FurnitureMimicMap.generated.h"

USTRUCT()
struct FFurnitureMimicMapEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UBlueprintGeneratedClass* Furniture;

	UPROPERTY(EditAnywhere)
	UBlueprintGeneratedClass* Mimic;
};

/**
 * 
 */
UCLASS()
class MIMICHUNT_API UFurnitureMimicMap : public UDataAsset
{
	GENERATED_BODY()
	public:
	UPROPERTY(EditAnywhere)
	TArray<FFurnitureMimicMapEntry> Entries;
	UFUNCTION(BlueprintCallable)
	UBlueprintGeneratedClass* GetFurniture(UBlueprintGeneratedClass* mimic);
	UFUNCTION(BlueprintCallable)
	UBlueprintGeneratedClass* GetMimic(UBlueprintGeneratedClass* furniture);
};
