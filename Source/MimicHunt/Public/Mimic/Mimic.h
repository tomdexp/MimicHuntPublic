// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mimic.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMimicBirth)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMimicChoseOrgans,const FChosenOrgansList&)
DECLARE_MULTICAST_DELEGATE(FOnMimicWake)
DECLARE_MULTICAST_DELEGATE(FOnMimicSleep)

USTRUCT()
struct FChosenOrganEntry
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString JointName;
	UPROPERTY()
	UBlueprintGeneratedClass* Organ;
	UPROPERTY()
	int RandomSeed;
};

//This way we're sure Unreal doesn't do voodoo things like Delta Serialization
USTRUCT()
struct FChosenOrgansList
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FChosenOrganEntry> ChosenOrgans;
};

UCLASS()
class MIMICHUNT_API AMimic : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMimic();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	UFUNCTION(BlueprintCallable,Category="Mimic")
	void MimicBirth();
	UFUNCTION(BlueprintCallable,Category="Mimic")
	void MimicWake();
	UFUNCTION(BlueprintCallable,Category="Mimic")
	void MimicSleep();
	UFUNCTION()
	void OnRep_ChosenOrgans();
	UFUNCTION()
	void OnRep_Test();
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_ChosenOrgans)
	FChosenOrgansList ChosenOrgans;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_Test)
	int TestRep;
	//TempChosenOrgans isn't replicated, when the time is right, we copy it to chosen organs and it gets replicated
	FChosenOrgansList TempChosenOrgans;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RegisterChosenOrgan(FString jointName, UBlueprintGeneratedClass* organ, int randomSeed);
	FOnMimicBirth OnMimicBirthDelegate;
	FOnMimicChoseOrgans OnMimicChoseOrgansDelegate;
	FOnMimicWake OnMimicWakeDelegate;
	FOnMimicSleep OnMimicSleepDelegate;
};
