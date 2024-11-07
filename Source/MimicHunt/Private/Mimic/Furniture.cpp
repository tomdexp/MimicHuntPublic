// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/Furniture.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"


// Sets default values
AFurniture::AFurniture()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates=true;
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
	if(!HasAuthority()) return;
	UBlueprintGeneratedClass* blueprintClass = Cast<UBlueprintGeneratedClass>(this->GetClass());
	FTransform spawnTransform = this->GetActorTransform();
	UWorld* world = this->GetWorld();
	AActor* newActor = world->SpawnActor<AActor>(FurnitureMimicMap->GetMimic(blueprintClass), spawnTransform);
	this->Destroy();
}

