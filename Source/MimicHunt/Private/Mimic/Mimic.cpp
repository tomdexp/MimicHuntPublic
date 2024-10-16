// Fill out your copyright notice in the Description page of Project Settings.

#include "Mimic/Mimic.h"

#include "Mimic/MimicCompositing/FurnitureJoint.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(LogMimic);

/* Mimics are comprised of several Static Meshes representing parts of the furniture: furniture chunks
 * Between these chunks are furniture joints, where mimic organs can spawn
 *
 */

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

	//We iterate a first time to create an map of all the furniture chunks by name and find the root to make it invisible
	TMap<FString, UStaticMeshComponent*> staticMeshMapByName=TMap<FString,UStaticMeshComponent*>();
	for(auto component : components)
	{
		if(component->GetClass()!=UStaticMeshComponent::StaticClass()) continue;
		auto staticMeshComponent =Cast<UStaticMeshComponent>(component);
		FString componentName=component->GetName();

		//The root is a static mesh component, we can't really replace it easily so we just make it invisible
		if (componentName == "ROOT")
		{
			staticMeshComponent->SetStaticMesh(nullptr);
			continue;
		}

		staticMeshMapByName.Add(componentName, staticMeshComponent);
	}

	 //We iterate on components to assign the Furniture Chunks to the correct joints
	 for (UActorComponent* component : components)
	 {
	 	FString componentName=component->GetName();
	 	
	 	if(component->GetClass()!=UFurnitureJoint::StaticClass()) continue;
	 	auto jointComponent=Cast<UFurnitureJoint>(component);

	 	if(!staticMeshMapByName.Contains(jointComponent->ParentChunkName))
	 	{
	 		UE_LOG(LogTemp, Error, TEXT("Parent chunk of %s, with name %s doesn't exist. Have you renamed it recently ? You should not do that")
				 , *component->GetName(), *jointComponent->ParentChunkName);
	 		continue;
	 	}
	 	jointComponent->ParentChunkComponent=staticMeshMapByName[jointComponent->ParentChunkName];
	 	
	 	if(!staticMeshMapByName.Contains(jointComponent->ChildChunkName))
	 	{
	 		UE_LOG(LogTemp, Error, TEXT("Child chunk of %s, with name %s doesn't exist. Have you renamed it recently ? You should not do that")
	 			, *component->GetName(), *jointComponent->ChildChunkName);
	 		continue;
	 	}
	 	jointComponent->ChildChunkComponent=staticMeshMapByName[jointComponent->ChildChunkName];
	}
}

// Called every frame
void AMimic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

