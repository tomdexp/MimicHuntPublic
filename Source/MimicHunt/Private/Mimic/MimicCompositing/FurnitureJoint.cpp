// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/MimicCompositing/FurnitureJoint.h"

#include "Mimic/MimicCompositing/AttachPoint.h"
#include "Mimic/MimicCompositing/MimicOrgan.h"
#include "Mimic/MimicCompositing/OrganBundle.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values for this component's properties
UFurnitureJoint::UFurnitureJoint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UFurnitureJoint::BeginPlay()
{
	Super::BeginPlay();
	ParentChunkMesh=ParentChunkComponent->GetStaticMesh();
	ChildChunkMesh=ChildChunkComponent->GetStaticMesh();
	Mimic=Cast<AMimic>(GetOwner());
	OnMimicBirth();
	Mimic->OnMimicWakeDelegate.AddUObject(this,&UFurnitureJoint::OnMimicWake);
	Mimic->OnMimicSleepDelegate.AddUObject(this,&UFurnitureJoint::OnMimicSleep);
}

// Called every frame
void UFurnitureJoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFurnitureJoint::OnMimicBirth()
{
	if(GetChildActor()!=nullptr)
	{
		UClass* clas=GetChildActor()->GetClass();
	}
	
	if(OrganBundles.Num()==0) return;
	//We choose which organ we take from the bundles
	TArray<FOrganBundleEntry>  ponderatedArray=TArray<FOrganBundleEntry>();
	for(auto bundle : OrganBundles)
	{
		if(bundle->AddEmptyEntry)
		{
			for (int i=0;i<bundle->RandomWeightOfEmptyEntry;i++)
			{
				ponderatedArray.Add(FOrganBundleEntry());
			}
		}
		for(auto organBundleEntry : bundle->OrganEntries)
		{
			for (int i=0;i<organBundleEntry.RandomWeight;i++)
			{
				ponderatedArray.Add(organBundleEntry);
			}
		}
	}
	if(ponderatedArray.Num()==0) return; 
	int32 randomIndex = FMath::RandRange(0, ponderatedArray.Num() - 1);
	FOrganBundleEntry randomEntry = ponderatedArray[randomIndex];
	if(randomEntry.Organ==nullptr) return;
	SetChildActorClass(randomEntry.Organ);
	Organ=Cast<AMimicOrgan>(GetChildActor());
	Organ->Initialize(Mimic,this);
	Organ->OnMimicBirth();
	_childChunkCachedRelativeTransform=ChildChunkComponent->GetRelativeTransform();
	OnMimicSleep();
}

void UFurnitureJoint::OnMimicWake()
{
	if(HideChildOnMimicWake)
	{
		ChildChunkComponent->SetStaticMesh(nullptr);
	}
	if(HideParentOnMimicWake)
	{
		ParentChunkComponent->SetStaticMesh(nullptr);
	}
	SetWorldScale3D(FVector::One());
	if(Organ==nullptr) return;
	UAttachPoint::PlaceChildRelativeToParent(Organ->GetRootComponent(),Organ->StartAttachPoint,ParentChunkComponent,this);
	UAttachPoint::PlaceChildRelativeToParent(ChildChunkComponent,EndAttachPoint,Organ->GetRootComponent(),Organ->EndAttachPoint);

	if(!Organ->IsPhysicked || Organ->PhysickedComponent==nullptr)
	{
		Organ->OnMimicWake();
		return;
	}

	FConstraintInstance ConstraintInstance;
	ConstraintInstance.SetDisableCollision(true);
	// Verrouiller toutes les translations
	ConstraintInstance.SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0.0f);
	ConstraintInstance.SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0.0f);
	ConstraintInstance.SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0.0f);
	
	// Verrouiller toutes les rotations (Swing et Twist)
	ConstraintInstance.SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, Organ->SwingAngle);
	ConstraintInstance.SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, Organ->SwingAngle);
	ConstraintInstance.SetAngularTwistLimit(EAngularConstraintMotion::ACM_Limited, Organ->TwistAngle);

	ConstraintInstance.SetSoftSwingLimitParams(true,Organ->Stiffness,Organ->Damping,0,0);
	ConstraintInstance.SetSoftTwistLimitParams(true,Organ->Stiffness,Organ->Damping,0,0);
	
	UPhysicsConstraintComponent* StartConstraintComp = NewObject<UPhysicsConstraintComponent>(ParentChunkComponent);
	StartConstraintComp->ConstraintInstance = ConstraintInstance;
	StartConstraintComp->SetWorldLocation(this->GetComponentLocation());
	StartConstraintComp->AttachToComponent(ParentChunkComponent, FAttachmentTransformRules::KeepWorldTransform);
	StartConstraintComp->SetConstrainedComponents(ParentChunkComponent,NAME_None,Organ->PhysickedComponent,NAME_None);

	if(ChildChunkComponent==nullptr || !Organ->MakeChildChunkPhysicked)
	{
		//Organ->PhysickedComponent->SetSimulatePhysics(true);
		Organ->OnMimicWake();
		return;
	}
	
	ConstraintInstance.SetDisableCollision(true);
	UPhysicsConstraintComponent* EndConstraintComp = NewObject<UPhysicsConstraintComponent>(ParentChunkComponent);
	EndConstraintComp->ConstraintInstance = ConstraintInstance;
	EndConstraintComp->SetWorldLocation(Organ->EndAttachPoint->GetComponentLocation());
	EndConstraintComp->AttachToComponent(Organ->PhysickedComponent, FAttachmentTransformRules::KeepWorldTransform);
	EndConstraintComp->SetConstrainedComponents(Organ->PhysickedComponent,NAME_None,ChildChunkComponent,NAME_None);
	Organ->PhysickedComponent->SetSimulatePhysics(true);
	ChildChunkComponent->SetSimulatePhysics(true);
	
	Organ->OnMimicWake();
}

void UFurnitureJoint::OnMimicSleep()
{
	if(HideChildOnMimicWake)
	{
		ChildChunkComponent->SetStaticMesh(ChildChunkMesh);
	}
	if(HideParentOnMimicWake)
	{
		ParentChunkComponent->SetStaticMesh(ParentChunkMesh);
	}
	SetWorldScale3D(FVector::Zero());
	if(Organ==nullptr) return;
	ChildChunkComponent->SetRelativeTransform(_childChunkCachedRelativeTransform);
	Organ->OnMimicSleep();
}
