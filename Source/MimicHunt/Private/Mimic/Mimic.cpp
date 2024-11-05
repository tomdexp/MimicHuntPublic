// Fill out your copyright notice in the Description page of Project Settings.

#include "Mimic/Mimic.h"

#include "Components/CapsuleComponent.h"
#include "Mimic/MimicCompositing/FurnitureJoint.h"
#include "Net/UnrealNetwork.h"
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
	bReplicates=true;
}

// Called when the game starts or when spawned
void AMimic::BeginPlay()
{
	Super::BeginPlay();
	MimicBirth();
}

void AMimic::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	CachedCapsuleRadius=GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	CachedCapsuleHalfHeight=GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	
	TSet<UActorComponent*> components=GetComponents();

	//We iterate a first time to create an map of all the furniture chunks by name, same for attachments and find the root to make it invisible
	//We use the occasion to set the collision right
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
			staticMeshComponent->SetRelativeLocation(FVector(0, 0, -GetSimpleCollisionHalfHeight()),false,nullptr,ETeleportType::TeleportPhysics);
			continue;
		}

		//This way legs won't step on furniture chunks
		staticMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Ignore);
		//staticMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
		//This way the capsule won't collide with the physicked part of the mimic
		staticMeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
		staticMeshComponent->UpdateCollisionProfile();
		
		staticMeshMapByName.Add(componentName, staticMeshComponent);
	}

	//We also change the collision behaviour on the capsule
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->UpdateCollisionProfile();

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

void AMimic::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Here we list the variables we want to replicate
	DOREPLIFETIME(AMimic, ChosenOrgans);
	DOREPLIFETIME(AMimic, IsAwake);
}

void AMimic::MimicBirth()
{
	OnMimicBirthEvent();
	OnMimicBirthDelegate.Broadcast();
	
	if(!HasAuthority())
	{
		//If ChosenOrgans isn't empty, it means it was already replicated before birth was called, so we need to call this delegates now
		//If it's empty, then it means it hasn't been replicated yet and the delegate will get called in OnRep_ChosenOrgans
		if(!ChosenOrgans.IsEmpty())
		{
			OnMimicChoseOrgansDelegate.Broadcast(ChosenOrgans);
			UE_LOG(LogTemp, Log,TEXT("PORDRACK Case !Hasauthority and !IsEmpty for %s"),*GetName());
			MimicSleep();
		}
		return;
	}
	ChosenOrgans=TempChosenOrgans;
	MimicSleep();
}

void AMimic::MimicWake()
{
	OnMimicWakeDelegate.Broadcast();
	GetCapsuleComponent()->SetCapsuleSize(CachedCapsuleRadius,CachedCapsuleHalfHeight);
	
	OnMimicWakeEvent();
	if(!HasAuthority()) return;
	IsAwake=true;
}

void AMimic::MimicSleep()
{
	OnMimicSleepDelegate.Broadcast();
	//We can't just activate/deactivate the capsule because it makes them "disappear" on the other client
	//GetCapsuleComponent()->SetCollisionEnabled()
	GetCapsuleComponent()->SetCapsuleSize(0,1);
	
	OnMimicSleepEvent();
	if(!HasAuthority())
	{
		UE_LOG(LogTemp, Log,TEXT("PORDRACK Sleep %s"),*GetName());
		return;
	};
	IsAwake=false;
}

void AMimic::OnRep_ChosenOrgans()
{
	//MimicBirth();
	OnMimicChoseOrgansDelegate.Broadcast(ChosenOrgans);
	UE_LOG(LogTemp, Log,TEXT("PORDRACK Case !Hasauthority and OnRepChoseOrgans for %s"),*GetName());
	MimicSleep();
}

void AMimic::OnRep_IsAwake()
{
	if(IsAwake)
		MimicWake();
	else
		MimicSleep();
}

// Called every frame
void AMimic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMimic::RegisterChosenOrgan(FString jointName, UBlueprintGeneratedClass* organ, int randomSeed)
{
	FChosenOrganEntry newEntry;
	newEntry.JointName=jointName;
	newEntry.Organ=organ;
	newEntry.RandomSeed=randomSeed;
	TempChosenOrgans.ChosenOrgans.Add(newEntry);
}

