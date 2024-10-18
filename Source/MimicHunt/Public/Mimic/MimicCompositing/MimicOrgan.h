// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MimicOrgan.generated.h"

UCLASS()
class MIMICHUNT_API AMimicOrgan : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMimicOrgan();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintImplementableEvent, Category = "MimicOrgan")
	void OnMimicBirthEvent(const AActor* Mimic);
	UFUNCTION(BlueprintImplementableEvent, Category = "MimicOrgan")
	void OnMimicWakeEvent(const AActor* Mimic);
	UFUNCTION(BlueprintImplementableEvent, Category = "MimicOrgan")
	void OnMimicSleepEvent(const AActor* Mimic);
	
	virtual void OnMimicBirth(const AActor* Mimic);
	virtual void OnMimicWake(const AActor* Mimic);
	virtual void OnMimicSleep(const AActor* Mimic);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
