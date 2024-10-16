// Fill out your copyright notice in the Description page of Project Settings.

#include "Mimic/Mimic.h"

#include "Mimic/MimicCompositing/FurnitureJoint.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(LogMimic);

// Sets default values
AMimic::AMimic()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMimic::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMimic::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	 TSet<UActorComponent*> components=GetComponents();

	 //We iterate on components to find the root and make it invisible
	 for (UActorComponent* component : components)
	 {
	 	if(component->GetClass()!=UStaticMeshComponent::StaticClass()) continue;
	 	auto staticMeshComponent =Cast<UStaticMeshComponent>(component);
	 	FString componentName=component->GetName();
	 	//The root is a static mesh component, we can't really replace it easily so we just make it invisible
	 	if(componentName=="ROOT")
	 	{
	 		staticMeshComponent->SetStaticMesh(nullptr);
	 		continue;
	 	}
	}
}

// Called every frame
void AMimic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

