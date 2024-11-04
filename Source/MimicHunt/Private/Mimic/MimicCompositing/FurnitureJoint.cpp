// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/MimicCompositing/FurnitureJoint.h"

#include "Mimic/MimicCompositing/AttachPoint.h"
#include "Mimic/MimicCompositing/MimicOrgan.h"
#include "Mimic/MimicCompositing/OrganBundle.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#if UE_EDITOR
#include "DrawDebugHelpers.h"
#endif

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
	//You can flip joints to invert the end and start attach point on a whim
	if(IsFlipped)
	{
		auto cache=ChildChunkComponent;
		ChildChunkComponent=ParentChunkComponent;
		ParentChunkComponent=cache;
		EndAttachPoint->AttachToComponent(ChildChunkComponent,FAttachmentTransformRules::KeepWorldTransform);
		this->AttachToComponent(ParentChunkComponent,FAttachmentTransformRules::KeepWorldTransform);
	}
	ParentChunkMesh=ParentChunkComponent->GetStaticMesh();
	if(ChildChunkComponent!=nullptr)
		ChildChunkMesh=ChildChunkComponent->GetStaticMesh();
	Mimic=Cast<AMimic>(GetOwner());
	Mimic->OnMimicWakeDelegate.AddUObject(this,&UFurnitureJoint::OnMimicWake);
	Mimic->OnMimicSleepDelegate.AddUObject(this,&UFurnitureJoint::OnMimicSleep);

	if(!GetOwner()->HasAuthority())
	{
		Mimic->OnMimicChoseOrgansDelegate.AddUObject(this, &UFurnitureJoint::OnMimicChoseOrgans);
	}
	InitializeJoint();
}

// Called every frame
void UFurnitureJoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFurnitureJoint::ChooseRandomOrgan()
{
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
	auto randomOrganBundleEntry = ponderatedArray[randomIndex];
	RandomSeed=FMath::RandRange(TNumericLimits<int>::Min(), TNumericLimits<int>::Max());

	ChosenRandomOrgan=randomOrganBundleEntry.Organ;
	Mimic->RegisterChosenOrgan(GetName(),ChosenRandomOrgan, RandomSeed);
}

void UFurnitureJoint::OnMimicChoseOrgans(const FChosenOrgansList& chosenOrgansList)
{
	for(auto entry : chosenOrgansList.ChosenOrgans)
	{
		if(GetName()!=entry.JointName) continue;
		ChosenRandomOrgan=entry.Organ;
		RandomSeed=entry.RandomSeed;
	}
	InitializeJoint();
}

void UFurnitureJoint::BeginDestroy()
{
	Super::BeginDestroy();

	if(_startConstraintComponent!=nullptr)
	{
		_startConstraintComponent->BreakConstraint();
	}
	if(_endConstraintComponent!=nullptr)
	{
		_endConstraintComponent->BreakConstraint();
	}
}

void UFurnitureJoint::InitializeJoint()
{
	//Compute the start to end direction on mimic birth, because calculating it on wake would make it altered by the shifting around of the wake up of the previous joints
	if(EndAttachPoint!=nullptr)
	{
		USceneComponent* jointStartAttachPoint=this;
		USceneComponent* jointEndAttachPoint=EndAttachPoint;
		if(IsFlipped)
		{
			jointEndAttachPoint=this;
			jointStartAttachPoint=EndAttachPoint;
		}
	
		StartToEndVector=jointEndAttachPoint->GetComponentLocation()-jointStartAttachPoint->GetComponentLocation();
	}

	if(GetOwner()->HasAuthority())
	{
		ChooseRandomOrgan();
	}
	
	if(ChildChunkComponent!=nullptr && ParentChunkComponent!=nullptr)
	{
		ChildChunkComponent->AttachToComponent(ParentChunkComponent,FAttachmentTransformRules::KeepWorldTransform);
	}

	if (ChildChunkComponent->GetAttachParent() == ParentChunkComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("%s is correctly attached to %s."),*ChildChunkComponent->GetName(),*ParentChunkComponent->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attachment failed."));
	}
	
	
	if(ChosenRandomOrgan==nullptr) return;
	SetChildActorClass(ChosenRandomOrgan);
	Organ=Cast<AMimicOrgan>(GetChildActor());
	Organ->Initialize(Mimic,this, RandomSeed);
	Organ->OnMimicBirth();
	if(ChildChunkComponent!=nullptr)
	{
		_childChunkCachedRelativeTransform=ChildChunkComponent->GetRelativeTransform();
		_childChunkCachedParent=ChildChunkComponent->GetAttachParent();
	}
	
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
	SetRelativeScale3D(FVector::One());
	if(Organ==nullptr) return;
	USceneComponent* jointStartAttachPoint=this;
	USceneComponent* jointEndAttachPoint=EndAttachPoint;
	if(IsFlipped)
	{
		jointEndAttachPoint=this;
		jointStartAttachPoint=EndAttachPoint;
	}

	//Place everything accordingly depending on type of organ
	if(Organ->IsSingle)
	{
		//Place the organ and child component accordingly, making sure their attachment points match
		UAttachPoint::PlaceChildRelativeToParent(Organ->GetRootComponent(),Organ->SingleAttachPoint,ParentChunkComponent,jointStartAttachPoint);
	}else
	{
		//Align the angle of the organ with the angle of the joints (regarding the direction of their Start attachment to end attachment vector)
		FVector organStartToEnd=Organ->EndAttachPoint->GetComponentLocation()-Organ->StartAttachPoint->GetComponentLocation();
		FVector targetDirection=StartToEndVector;
		if(ManuallySetJointDirection)
		{
			targetDirection=ManualJointDirection;
		}
		
		FRotator lookAtRotation = FQuat::FindBetweenVectors(organStartToEnd,targetDirection).Rotator();
		Organ->GetRootComponent()->SetWorldRotation(lookAtRotation);
		
		//Place the organ and child component accordingly, making sure their attachment points match
		UAttachPoint::PlaceChildRelativeToParent(Organ->GetRootComponent(),Organ->StartAttachPoint,ParentChunkComponent,jointStartAttachPoint);
		UAttachPoint::PlaceChildRelativeToParent(ChildChunkComponent,jointEndAttachPoint,Organ->GetRootComponent(),Organ->EndAttachPoint);
		ChildChunkComponent->AttachToComponent(Organ->EndAttachPoint,FAttachmentTransformRules::KeepWorldTransform);
	}
	

	//If the organ is physicked, we start the physicking
	if(!Organ->IsPhysicked || Organ->PhysickedComponent==nullptr)
	{
		Organ->OnMimicWake();
		return;
	}

	//Create the physic parameters, used for both physics joints
	FConstraintInstance ConstraintInstance;
	ConstraintInstance.SetDisableCollision(true);
	ConstraintInstance.SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0.0f);
	ConstraintInstance.SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0.0f);
	ConstraintInstance.SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0.0f);
	ConstraintInstance.SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, Organ->SwingAngle);
	ConstraintInstance.SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, Organ->SwingAngle);
	ConstraintInstance.SetAngularTwistLimit(EAngularConstraintMotion::ACM_Limited, Organ->TwistAngle);

	ConstraintInstance.SetSoftSwingLimitParams(true,Organ->Stiffness,Organ->Damping,0,0);
	ConstraintInstance.SetSoftTwistLimitParams(true,Organ->Stiffness,Organ->Damping,0,0);

	//Create the physic joint between joint's parent chunk and the organ
	if(_startConstraintComponent==nullptr)
	{
		_startConstraintComponent = NewObject<UPhysicsConstraintComponent>(ParentChunkComponent);
	}
	
	_startConstraintComponent->ConstraintInstance = ConstraintInstance;
	_startConstraintComponent->SetWorldLocation(jointStartAttachPoint->GetComponentLocation());
	_startConstraintComponent->AttachToComponent(ParentChunkComponent, FAttachmentTransformRules::KeepWorldTransform);
	_startConstraintComponent->SetConstrainedComponents(ParentChunkComponent,NAME_None,Organ->PhysickedComponent,NAME_None);

	if(ChildChunkComponent==nullptr || !Organ->MakeChildChunkPhysicked || Organ->IsSingle)
	{
		Organ->OnMimicWake();
		return;
	}
	
	//Create the physic joint between the organ and the joint's child chunk
	if(_endConstraintComponent==nullptr)
	{
		_endConstraintComponent = NewObject<UPhysicsConstraintComponent>(ParentChunkComponent);
	}
	_endConstraintComponent->ConstraintInstance = ConstraintInstance;
	_endConstraintComponent->SetWorldLocation(Organ->EndAttachPoint->GetComponentLocation());
	_endConstraintComponent->AttachToComponent(Organ->PhysickedComponent, FAttachmentTransformRules::KeepWorldTransform);
	_endConstraintComponent->SetConstrainedComponents(Organ->PhysickedComponent,NAME_None,ChildChunkComponent,NAME_None);
	
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
	
	if(Organ==nullptr)
	{
		SetRelativeScale3D(FVector::Zero());
		return;
	}
	
	if(_startConstraintComponent!=nullptr)
	{
		_startConstraintComponent->BreakConstraint();
		_startConstraintComponent->DestroyComponent();
		_startConstraintComponent=nullptr;
	}
	if(_endConstraintComponent!=nullptr)
	{
		_endConstraintComponent->BreakConstraint();
		_endConstraintComponent->DestroyComponent();
		_endConstraintComponent=nullptr;
	}

	if(Organ->IsPhysicked && Organ->PhysickedComponent!=nullptr)
	{
		Organ->PhysickedComponent->SetSimulatePhysics(false);
		Organ->AttachToComponent(this,FAttachmentTransformRules::SnapToTargetIncludingScale);
		Organ->PhysickedComponent->AttachToComponent(Organ->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
		
	}
	
	if(ChildChunkComponent!=nullptr)
	{
		ChildChunkComponent->AttachToComponent(_childChunkCachedParent, FAttachmentTransformRules::KeepWorldTransform);
		ChildChunkComponent->SetRelativeTransform(_childChunkCachedRelativeTransform);
		ChildChunkComponent->SetSimulatePhysics(false);
	}

	SetRelativeScale3D(FVector::Zero());
	Organ->OnMimicSleep();
}
