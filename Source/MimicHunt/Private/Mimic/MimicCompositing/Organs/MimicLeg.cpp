// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/MimicCompositing/Organs/MimicLeg.h"


// Sets default values
AMimicLeg::AMimicLeg()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMimicLeg::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMimicLeg::OnMimicBirth(const AActor* Mimic)
{
	Super::OnMimicBirth(Mimic);
}

void AMimicLeg::OnMimicWake(const AActor* Mimic)
{
	Super::OnMimicWake(Mimic);
}

void AMimicLeg::OnMimicSleep(const AActor* Mimic)
{
	Super::OnMimicSleep(Mimic);
}

// Called every frame
void AMimicLeg::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

