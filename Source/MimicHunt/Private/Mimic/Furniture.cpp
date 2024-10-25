// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/Furniture.h"


// Sets default values
AFurniture::AFurniture()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFurniture::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFurniture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFurniture::TurnToMimic()
{
	UBlueprintGeneratedClass* BlueprintClass = Cast<UBlueprintGeneratedClass>(this->GetClass());
	FTransform SpawnTransform = this->GetActorTransform();
	UWorld* World = this->GetWorld();
	AActor* NewActor = World->SpawnActor<AActor>(FurnitureMimicMap->GetMimic(BlueprintClass), SpawnTransform);
	
	this->Destroy();
}

