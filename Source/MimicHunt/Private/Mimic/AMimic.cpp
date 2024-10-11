// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/AMimic.h"

#include "Mimic/MimicCompositing/FurnitureJoint.h"
#include "Utils/LLog.h"

DEFINE_LOG_CATEGORY(LogMimic);

// Sets default values
AAMimic::AAMimic()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

//Will remove the ballising parts of the static meshs of the Mimic FBX and keep the part that says what chunk of the furniture the solid mesh is connected to
FString ExtractLinkedFurnitureChunkPart(const FString& Input)
{
	FRegexPattern Pattern(TEXT("(?:MIMIC_[0-9]*_(?:END)_|(?:START)_)(.*)"));
	FRegexMatcher Matcher(Pattern, Input);

	if (Matcher.FindNext())
	{
		return Matcher.GetCaptureGroup(1); // Renvoie le premier groupe capturé (Chair_Top_R)
	}
    
	return FString(); // Si aucune correspondance n'est trouvée
}


//Will turn all the quirks added to the fbx into actual components that we can use
void AAMimic::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	TSet<UActorComponent*> components=GetComponents();
	//Once a component of a pair is treated, we store it here so that we can quickly dismiss the second of the pair
	TArray<FString> treatedPairs;
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
		//all the mesh that aren't really mesh but beacons for us should start with MIMIC
		if(componentName.Left(5)!="MIMIC")continue;
		auto parentComponent = Cast<UStaticMeshComponent>(staticMeshComponent->GetAttachParent());
		FString noPrefixName=ExtractLinkedFurnitureChunkPart(componentName);
		
		staticMeshComponent->SetStaticMesh(nullptr);
		UE_LOG(LogTemp,Log,TEXT("%s converted, name without prefix: %s"),*componentName,*noPrefixName);
		// MIMIC_1_END_Chair_Top_R
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

