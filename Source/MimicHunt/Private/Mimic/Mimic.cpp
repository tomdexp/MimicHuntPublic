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

	//We iterate a first time to create an map of all the furniture chunks by name, same for attachments and find the root to make it invisible
	TMap<FString, USceneComponent*> attachmentsMapByName=TMap<FString, USceneComponent*>();
	TMap<FString, UStaticMeshComponent*> staticMeshMapByName=TMap<FString,UStaticMeshComponent*>();
	for(auto component : components)
	{
		FString componentName=component->GetName();
		if(component->GetClass()!=USceneComponent::StaticClass())
		{
			USceneComponent* sceneComponent=Cast<USceneComponent>(component);
			attachmentsMapByName.Add(componentName,sceneComponent);
		}
		if(component->GetClass()!=UStaticMeshComponent::StaticClass()) continue;
		auto staticMeshComponent =Cast<UStaticMeshComponent>(component);

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
		jointComponent->Mimic=this;
		if(attachmentsMapByName.Contains(jointComponent->EndAttachPointName))
		{
			jointComponent->EndAttachPoint=attachmentsMapByName[jointComponent->EndAttachPointName];
		}
		jointComponent->ParentChunkComponent=Cast<UStaticMeshComponent>(jointComponent->GetAttachParent());
		if(jointComponent->EndAttachPoint==nullptr) continue;
		jointComponent->ChildChunkComponent=Cast<UStaticMeshComponent>(jointComponent->EndAttachPoint->GetAttachParent());
	}
}

void AMimic::MimicBirth()
{
	OnMimicBirthDelegate.Broadcast();
}

void AMimic::MimicWake()
{
	OnMimicWakeDelegate.Broadcast();
}

void AMimic::MimicSleep()
{
	OnMimicSleepDelegate.Broadcast();
}

// Called every frame
void AMimic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

