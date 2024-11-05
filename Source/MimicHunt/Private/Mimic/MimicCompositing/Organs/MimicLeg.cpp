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

void AMimicLeg::OnMimicBirth()
{
	FVector mimicRootPosition=Mimic->GetActorLocation();
	//Find the direction of Mimic to self
	FVector rootToJoint=GetActorLocation()-mimicRootPosition;
	FVector direction=rootToJoint.GetSafeNormal();
	_legStartAngle=FMath::RadiansToDegrees(atan2(direction.Y,direction.X));
	Super::OnMimicBirth();
}

void AMimicLeg::OnMimicWake()
{
	Super::OnMimicWake();
}

void AMimicLeg::OnMimicSleep()
{
	OnDeactivateLeg();
	Super::OnMimicSleep();
}

// Called every frame
void AMimicLeg::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

