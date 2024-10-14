// Fill out your copyright notice in the Description page of Project Settings.

#include "Mimic/AMimic.h"

#include "Mimic/MimicCompositing/FurnitureJoint.h"
#include "Utils/LLog.h"

DEFINE_LOG_CATEGORY(LogMimic);

/*--------------------------------------------------
 * A quick nomenclature guide: The actual, here to stay, meshs are called "Furniture Chunks"
 * The ones added to be replaced by code are called beacons
 * The "MIMIC_1_END_" part of a beacon's name is called a beacon's prefix
 --------------------------------------------------*/

// Sets default values
AAMimic::AAMimic()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

//Functions here to extract various informations from beacon names
#pragma region "REGEX function for beacons names"
//Will remove the ballising parts of the static meshs of the Mimic FBX and keep the part that says what chunk of the furniture the solid mesh is connected to
FString ExtractLinkedFurnitureChunkPart(const FString& Input)
{
	FRegexPattern Pattern(TEXT("(?:MIMIC_[0-9]*_(?:END)_|(?:START)_)(.*)"));
	FRegexMatcher Matcher(Pattern, Input);

	if (Matcher.FindNext())
	{
		return Matcher.GetCaptureGroup(1);
	}
    
	return FString();
}

//If you feed it MIMIC_1_END_Chair_Top_R, it will give back MIMIC_1_START_Chair_Top_R and vice versa
FString FindComplementaryBeacon(const FString& Input)
{
	FRegexPattern Pattern(TEXT("(MIMIC_[0-9]*_)(END|START)(.*)"));
	FRegexMatcher Matcher(Pattern, Input);

	if (Matcher.FindNext())
	{
		FString ComplementaryBeaconName="";
		ComplementaryBeaconName+=Matcher.GetCaptureGroup(1);
		ComplementaryBeaconName+=(Matcher.GetCaptureGroup(2)=="START")?"END":"START";
		ComplementaryBeaconName+=Matcher.GetCaptureGroup(3);
		return ComplementaryBeaconName;
	}
    
	return FString();
}

//Returns true if the beacon's name submitted is one of a start beacon
bool IsStartBeacon(const FString& Input)
{
	FRegexPattern Pattern(TEXT("(?:MIMIC_[0-9]*_)(END|START)(?:.*)"));
	FRegexMatcher Matcher(Pattern, Input);

	if (Matcher.FindNext())
	{
		FString matchCaptureGroup = Matcher.GetCaptureGroup(1);
		bool isStart=matchCaptureGroup=="START";
		return isStart;
	}
    
	return false;
}
#pragma endregion




//Will replace beacons with components
void AAMimic::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	TSet<UActorComponent*> components=GetComponents();
	//We store beacons to delete them at a lower cost once we're done
	TQueue<UActorComponent*> beacons;

	//We do a first pass to build a map of component name -> component for the furniture parts (components with no special prefix)
	TMap<FString, UActorComponent*> componentsByName=TMap<FString, UActorComponent*>();
	for (UActorComponent* component : components)
	{
		FString componentName=component->GetName();
		componentsByName.Add(component->GetName(), component);
	}
	
	//We iterate on components to find all the pairs of START and END where we must add a joint
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
		//We are only looking for beacons here, if no prefix, it's a furniture chunk and we should leave
		if(componentName.Left(5)!="MIMIC")continue;
		beacons.Enqueue(component);
		//We don't want to deal with end beacons first
		if(!IsStartBeacon(componentName)) continue;
		
		FString linkedChunkName=ExtractLinkedFurnitureChunkPart(componentName);
		
		auto parentComponent = Cast<UStaticMeshComponent>(staticMeshComponent->GetAttachParent());
		FString complementaryBeaconName=FindComplementaryBeacon(componentName);
		
		if(!componentsByName.Contains(complementaryBeaconName))
		{
			UE_LOG(LogTemp,Error,TEXT("%s's name refers to a part named %s, but it doesnt exist"),*componentName,*complementaryBeaconName);
			continue;
		}
		auto complementaryBeacon=componentsByName[complementaryBeaconName];
		auto complementaryBeaconParentComponent=Cast<UStaticMeshComponent>(Cast<UStaticMeshComponent>(complementaryBeacon)->GetAttachParent());

		UFurnitureJoint* furnitureJoint = NewObject<UFurnitureJoint>();
		//furnitureJoint->SetupAttachment(parentComponent);
		furnitureJoint->ConnectionPoint1=parentComponent;
		furnitureJoint->ConnectionPoint2=complementaryBeaconParentComponent;
		furnitureJoint->CreationMethod=EComponentCreationMethod::UserConstructionScript;
		furnitureJoint->SetMobility(EComponentMobility::Movable);
		furnitureJoint->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		furnitureJoint->SetRelativeTransform(staticMeshComponent->GetRelativeTransform());
		furnitureJoint->RegisterComponent();
		furnitureJoint->SetActive(true);
		UE_LOG(LogTemp, Log, TEXT("Creating and attaching component: %s"),*furnitureJoint->GetFullName())
		this->RegisterAllComponents();
	}

	UActorComponent* beaconToDelete;
	while(beacons.Dequeue(beaconToDelete))
	{
		beaconToDelete->DestroyComponent();
	}
}

// Called when the game starts or when spawned
void AAMimic::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAMimic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

