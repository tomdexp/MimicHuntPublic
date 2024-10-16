// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mimic.generated.h"

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
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
