// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mimic/Mimic.h"
#include "MimicOrgan.generated.h"

UCLASS()
class MIMICHUNT_API AMimicOrgan : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMimicOrgan();
	virtual void OnMimicBirth();
	virtual void OnMimicWake();
	virtual void OnMimicSleep();
	
	void Initialize(AMimic* mimic, UChildActorComponent* ownerComponent);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintImplementableEvent, Category = "MimicOrgan")
	void OnMimicBirthEvent();
	UFUNCTION(BlueprintImplementableEvent, Category = "MimicOrgan")
	void OnMimicWakeEvent();
	UFUNCTION(BlueprintImplementableEvent, Category = "MimicOrgan")
	void OnMimicSleepEvent();
	
	UPROPERTY(BlueprintReadOnly)
	UChildActorComponent* _ownerComponent=nullptr;
	UPROPERTY(BlueprintReadOnly)
	AMimic* _mimic=nullptr;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
