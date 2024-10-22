// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/MimicCompositing/MimicOrgan.h"

#include "Mimic/MimicCompositing/AttachPoint.h"
#include "Mimic/MimicCompositing/FurnitureJoint.h"


// Sets default values
AMimicOrgan::AMimicOrgan()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMimicOrgan::BeginPlay()
{
	Super::BeginPlay();
}

void AMimicOrgan::Initialize(AMimic* mimic, UChildActorComponent* ownerComponent)
{
	_mimic = mimic;
	_ownerComponent = ownerComponent;
	TArray<UActorComponent*> attachPointsActorComponents=K2_GetComponentsByClass(UAttachPoint::StaticClass());
	for (UActorComponent* attachPointActorComponent : attachPointsActorComponents)
	{
		auto attachPoint=Cast<UAttachPoint>(attachPointActorComponent);
		if(attachPoint->IsStart)
		{
			StartAttachPoint=attachPoint;
			continue;
		}
		EndAttachPoint=attachPoint;
	}
}

void AMimicOrgan::OnMimicBirth()
{
	OnMimicBirthEvent();
}

void AMimicOrgan::OnMimicWake()
{
	OnMimicWakeEvent();
}

void AMimicOrgan::OnMimicSleep()
{
	OnMimicSleepEvent();
}

// Called every frame
void AMimicOrgan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

