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

#if WITH_EDITOR
#include "SubobjectDataSubsystem.h"
#include "Editor.h"

// void AMimic::ComputeFBXIntoFinalForm()
// {
// 	// TSet<UActorComponent*> components=GetComponents();
// 	// //We store beacons to delete them at a lower cost once we're done
// 	// TQueue<UActorComponent*> beacons;
// 	//
// 	// //We do a first pass to build a map of component name -> component for the furniture parts (components with no special prefix)
// 	// TMap<FString, UActorComponent*> componentsByName=TMap<FString, UActorComponent*>();
// 	// for (UActorComponent* component : components)
// 	// {
// 	// 	FString componentName=component->GetName();
// 	// 	componentsByName.Add(component->GetName(), component);
// 	// }
// 	//
// 	// //We iterate on components to find all the pairs of START and END where we must add a joint
// 	// for (UActorComponent* component : components)
// 	// {
// 	// 	if(component->GetClass()!=UStaticMeshComponent::StaticClass()) continue;
// 	// 	auto staticMeshComponent =Cast<UStaticMeshComponent>(component);
// 	// 	FString componentName=component->GetName();
// 	// 	//The root is a static mesh component, we can't really replace it easily so we just make it invisible
// 	// 	if(componentName=="ROOT")
// 	// 	{
// 	// 		staticMeshComponent->SetStaticMesh(nullptr);
// 	// 		continue;
// 	// 	}
// 	// 	//We are only looking for beacons here, if no prefix, it's a furniture chunk and we should leave
// 	// 	if(componentName.Left(5)!="MIMIC")continue;
// 	// 	beacons.Enqueue(component);
// 	// 	
// 	// 	if(!IsStartBeacon(componentName)) continue;
// 	// 	
// 	// 	FString linkedChunkName=ExtractLinkedFurnitureChunkPart(componentName);
// 	// 	
// 	// 	auto parentComponent = Cast<UStaticMeshComponent>(staticMeshComponent->GetAttachParent());
// 	// 	FString complementaryBeaconName=FindComplementaryBeacon(componentName);
// 	// 	
// 	// 	if(!componentsByName.Contains(complementaryBeaconName))
// 	// 	{
// 	// 		UE_LOG(LogTemp,Error,TEXT("%s's name refers to a part named %s, but it doesnt exist"),*componentName,*complementaryBeaconName);
// 	// 		continue;
// 	// 	}
// 	// 	auto complementaryBeacon=componentsByName[complementaryBeaconName];
// 	// 	auto complementaryBeaconParentComponent=Cast<UStaticMeshComponent>(Cast<UStaticMeshComponent>(complementaryBeacon)->GetAttachParent());
// 	//
// 	// 	// UFurnitureJoint* furnitureJoint = NewObject<UFurnitureJoint>();
// 	// 	// //furnitureJoint->SetupAttachment(parentComponent);
// 	// 	// furnitureJoint->ConnectionPoint1=parentComponent;
// 	// 	// furnitureJoint->ConnectionPoint2=complementaryBeaconParentComponent;
// 	// 	// furnitureJoint->CreationMethod=EComponentCreationMethod::UserConstructionScript;
// 	// 	// furnitureJoint->SetMobility(EComponentMobility::Movable);
// 	// 	// furnitureJoint->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
// 	// 	// furnitureJoint->SetRelativeTransform(staticMeshComponent->GetRelativeTransform());
// 	// 	// //furnitureJoint->RegisterComponent();
// 	// 	// furnitureJoint->SetActive(true);
// 	// 	//UE_LOG(LogTemp, Log, TEXT("Creating and attaching component: %s"),*furnitureJoint->GetFullName())
// 	// 	//this->RegisterAllComponents();
// 	//}
//
// 	// UActorComponent* beaconToDelete;
// 	// while(beacons.Dequeue(beaconToDelete))
// 	// {
// 	// 	beaconToDelete->DestroyComponent();
// 	// }
// }
#endif

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

