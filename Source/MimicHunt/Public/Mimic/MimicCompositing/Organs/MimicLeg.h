// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mimic/MimicCompositing/MimicOrgan.h"
#include "MimicLeg.generated.h"

UCLASS()
class MIMICHUNT_API AMimicLeg : public AMimicOrgan
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMimicLeg();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnMimicBirth(const AActor* Mimic) override;
	virtual void OnMimicWake(const AActor* Mimic) override;
	virtual void OnMimicSleep(const AActor* Mimic) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
