// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MHMimicCharacter.h"
#include "Core/MHLivingBeing.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
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
	FString JointName="";
	UPROPERTY()
	UBlueprintGeneratedClass* Organ=nullptr;
	UPROPERTY()
	int RandomSeed=0;
};

//This way we're sure Unreal doesn't do voodoo things like Delta Serialization
USTRUCT()
struct FChosenOrgansList
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FChosenOrganEntry> ChosenOrgans=TArray<FChosenOrganEntry>();
	bool IsEmpty() const
	{
		return ChosenOrgans.Num() == 0;
	}
};

UCLASS()
class MIMICHUNT_API AMimic : public AMHMimicCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMimic();
	UFUNCTION(BlueprintCallable,Category="Mimic")
	void MimicBirth();
	UFUNCTION(BlueprintCallable,Category="Mimic")
	void MimicWake();
	UFUNCTION(BlueprintCallable,Category="Mimic")
	void MimicSleep();

	UFUNCTION(BlueprintImplementableEvent, Category = "Mimic")
	void OnMimicBirthEvent();
	UFUNCTION(BlueprintImplementableEvent, Category = "Mimic")
	void OnMimicWakeEvent();
	UFUNCTION(BlueprintImplementableEvent, Category = "Mimic")
	void OnMimicSleepEvent();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	UFUNCTION()
	void OnRep_ChosenOrgans();
	UPROPERTY(ReplicatedUsing=OnRep_ChosenOrgans)
	FChosenOrgansList ChosenOrgans;
	//TempChosenOrgans isn't replicated, when the time is right, we copy it to chosen organs and it gets replicated
	FChosenOrgansList TempChosenOrgans;
	void DeactivateCapsule();
	void ActivateCapsule();
	UPROPERTY()
	int CachedCapsuleRadius;
	UPROPERTY()
	int CachedCapsuleHalfHeight;
	UPROPERTY(BlueprintReadOnly)
	USceneComponent* MimicRoot;

	UFUNCTION()
	void OnRep_IsAwake();
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_IsAwake)
	bool IsAwake;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RegisterChosenOrgan(FString jointName, UBlueprintGeneratedClass* organ, int randomSeed);
	FOnMimicBirth OnMimicBirthDelegate;
	FOnMimicChoseOrgans OnMimicChoseOrgansDelegate;
	FOnMimicWake OnMimicWakeDelegate;
	FOnMimicSleep OnMimicSleepDelegate;
};
