// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MimicOrgan.h"
#include "Engine/DataAsset.h"
#include "OrganBundle.generated.h"

USTRUCT()
struct FOrganBundleEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UBlueprintGeneratedClass* Organ;

	//Higher it is, the likelier it is to spawn
	UPROPERTY(EditAnywhere)
	int RandomWeight=1;
};

/**
 * 
 */
UCLASS()
class MIMICHUNT_API UOrganBundle : public UDataAsset
{
	GENERATED_BODY()
	public:
	//If true, there's a probability this bundle spit out no organ
	UPROPERTY(EditAnywhere)
	bool AddEmptyEntry;
	UPROPERTY(EditAnywhere, meta=(EditCondition="AddEmptyEntry", EditConditionHides))
	//If previous is true, the higher the weight, the likelier no organ will be spat out
	int RandomWeightOfEmptyEntry=1;
	UPROPERTY(EditAnywhere)
	TArray<FOrganBundleEntry> OrganEntries;
};
