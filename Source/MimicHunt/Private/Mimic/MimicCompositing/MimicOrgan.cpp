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
	TArray<UActorComponent*> attachPointsActorComponents = K2_GetComponentsByClass(UAttachPoint::StaticClass());
	for (UActorComponent* attachPointActorComponent : attachPointsActorComponents)
	{
		auto attachPoint = Cast<UAttachPoint>(attachPointActorComponent);
		if (attachPoint->AttachPointType==EAttachPointType::Single)
		{
			SingleAttachPoint = attachPoint;
			IsSingle = true;
			continue;
		}
		IsSingle=false;
		if (attachPoint->AttachPointType==EAttachPointType::Start)
		{
			StartAttachPoint = attachPoint;
			continue;
		}
		EndAttachPoint = attachPoint;
	}
	if (!IsPhysicked) return;
	TArray<UActorComponent*> staticMeshComponents = K2_GetComponentsByClass(UStaticMeshComponent::StaticClass());
	for (UActorComponent* staticMeshComponent : staticMeshComponents)
	{
		if (staticMeshComponent->GetName() != PhysickedComponentName) continue;
		PhysickedComponent = Cast<UStaticMeshComponent>(staticMeshComponent);
		break;
	}
	if (PhysickedComponent == nullptr)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("Could not find a component named %s in Organ %s, a physicked organ needs to have a PhysickedComponentName pointing to its main StaticMeshComponent"),
		       *PhysickedComponentName, *GetName()
		       );
	}
}

void AMimicOrgan::OnMimicBirth()
{
	OnMimicBirthEvent();
}

void AMimicOrgan::OnMimicWake()
{
	IsAwake=true;
	OnMimicWakeEvent();
}

void AMimicOrgan::OnMimicSleep()
{
	IsAwake=false;
	OnMimicSleepEvent();
}

// Called every frame
void AMimicOrgan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
