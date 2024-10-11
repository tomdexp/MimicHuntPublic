// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AMimic.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMimic, Log, All);

UCLASS()

class MIMICHUNT_API AAMimic : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAMimic();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
