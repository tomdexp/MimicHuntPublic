// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FurnitureMimicMap.h"
#include "GameFramework/Actor.h"
#include "Furniture.generated.h"

UCLASS()
class MIMICHUNT_API AFurniture : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFurniture();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere)
	UFurnitureMimicMap* FurnitureMimicMap;
	UFUNCTION(BlueprintCallable)
	void TurnToMimic();
};
