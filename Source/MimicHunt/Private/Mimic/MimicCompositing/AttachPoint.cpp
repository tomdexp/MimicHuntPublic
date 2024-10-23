// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/MimicCompositing/AttachPoint.h"


// Sets default values for this component's properties
UAttachPoint::UAttachPoint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttachPoint::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAttachPoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAttachPoint::PlaceChildRelativeToParent(USceneComponent* Child,USceneComponent* ChildAttachPoint,
	const USceneComponent* Parent, const USceneComponent* ParentAttachPoint, bool RotateChild)
{
	if(RotateChild)
		Child->SetWorldRotation(Parent->GetComponentRotation());
	//We can't just get the relative location with ChildAttachPoint->GetRelativeLocation because the coordinate would be scaled locally
	FVector childAttachPointRelativeLocation=(ChildAttachPoint->GetComponentLocation()-Child->GetComponentLocation()); ;
	FVector NewChildLocation=ParentAttachPoint->GetComponentLocation()-childAttachPointRelativeLocation;
	UE_LOG(LogTemp,Log,TEXT("child attach point relative location:%s"),*childAttachPointRelativeLocation.ToString());
	Child->SetWorldLocation(NewChildLocation);
}

