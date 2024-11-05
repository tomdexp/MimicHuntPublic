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
	virtual void OnMimicBirth() override;
	virtual void OnMimicWake() override;
	virtual void OnMimicSleep() override;
	//The rotation we apply to the leg on start based on its position relative to the mimic 
	UPROPERTY(BlueprintReadOnly)
	float _legStartAngle;
	UFUNCTION(BlueprintImplementableEvent, Category = "Mimic")
	void OnDeactivateLeg();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
