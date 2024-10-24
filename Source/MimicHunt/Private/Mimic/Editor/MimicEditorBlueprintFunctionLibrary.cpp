// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/Editor/MimicEditorBlueprintFunctionLibrary.h"
#include "Mimic/Mimic.h"
#include "Mimic/MimicCompositing/AttachPoint.h"
#include "Mimic/MimicCompositing/FurnitureJoint.h"
#include "Utils/LLog.h"
LL_FILE_CVAR(LogMimicEditor);

/*--------------------------------------------------
 * A quick nomenclature guide: The actual, here to stay, meshs are called "Furniture Chunks"
 * The ones added to be replaced by code are called beacons
 * The "MIMIC_1_END_" part of a beacon's name is called a beacon's prefix
 --------------------------------------------------*/

FString BoolToString(bool value)
{
	return value ? "true" : "false";
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
		FString ComplementaryBeaconName = "";
		ComplementaryBeaconName += Matcher.GetCaptureGroup(1);
		ComplementaryBeaconName += (Matcher.GetCaptureGroup(2) == "START") ? "END" : "START";
		ComplementaryBeaconName += Matcher.GetCaptureGroup(3);
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
		bool isStart = matchCaptureGroup == "START";
		return isStart;
	}

	return false;
}
#pragma endregion

//SceneComponent->GetComponentTransform doesn't work in editor scripting (always return identity), so we're "recoding" it
//I'm not sure this function works if there are multiple scale change and rotations etc. But it works perfectly for object where the root is scaled and there's only translations
FTransform GetWorldTransform(FSubobjectDataHandle Handle, USubobjectDataSubsystem* SubobjectDataSubsystem)
{
	FTransform transform = FTransform::Identity;
	auto data = Handle.GetData();

	// We start with an identity transform and accumulate the relative transforms to get the world transform
	do
	{
		if (!data->IsSceneComponent()) break;
		auto parentData = data->GetParentHandle().GetData();
		auto dataSceneComponent = Cast<USceneComponent>(data->GetObject());
		transform = transform.GetRelativeTransform(dataSceneComponent->GetRelativeTransform());
		data = parentData;
	} while (data->HasParent());

	transform=(transform).Inverse();
	transform.SetTranslation(transform.GetTranslation()*transform.GetScale3D());
	
	return transform;
}

void UMimicEditorBlueprintFunctionLibrary::ComputeMimicBlueprint(UBlueprint* Blueprint)
{
	if (!Blueprint)
	{
		UE_LOG(LogTemp, Error, TEXT("Blueprint is null."));
		return;
	}

	if (!Blueprint->GeneratedClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Blueprint has no GeneratedClass."));
		return;
	}

	UObject* DefaultObject = Blueprint->GeneratedClass->GetDefaultObject();

	if (!DefaultObject)
	{
		UE_LOG(LogTemp, Error, TEXT("Blueprint has no DefaultObject."));
		return;
	}

	//AMimic is the parent class of the blueprint
	AMimic* mimic = Cast<AMimic>(DefaultObject);

	USubobjectDataSubsystem* subobjectSubsystem = GEditor->GetEngineSubsystem<USubobjectDataSubsystem>();

	if (!subobjectSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("SubobjectDataSubsystem not found."));
		return;
	}

	//We'll contain all the handle for the components, in SubobjectData form
	TArray<FSubobjectDataHandle> outSubobjectDataHandles;
	subobjectSubsystem->GatherSubobjectData(DefaultObject, outSubobjectDataHandles);

	//A lot of add/delete handle need a FSubobjectDataHandle of the "owner", I'm not sure how it works, so we'll take the first component we find, and assume its root is the owner
	//This way context handle will point to the DefaultSceneRoot
	FSubobjectDataHandle contextHandle;
	bool contextHandleDefined = false;

	//We do a first pass to build a map of component name -> component Data handle
	TMap<FString, FSubobjectDataHandle> componentsByName = TMap<FString, FSubobjectDataHandle>();
	for (FSubobjectDataHandle handle : outSubobjectDataHandles)
	{
		if (!contextHandleDefined)
		{
			contextHandleDefined = true;
			contextHandle = subobjectSubsystem->FindSceneRootForSubobject(handle);
		}
		auto subobjectData = handle.GetData();
		if (!subobjectData->IsSceneComponent()) continue;

		FString componentName = subobjectData->GetAssetName().ToString();
		componentsByName.Add(componentName, handle);
	}

	//We store beacons to delete them at a lower cost once we're done
	TArray<FSubobjectDataHandle> beaconsHandles;

	//For some reason some handles come up multiple time in the array, so we build an Array of already treated handles
	TArray<FSubobjectDataHandle> iteratedHandles=TArray<FSubobjectDataHandle>();
	//We iterate over subobjects to replace beacons with FurnitureJoint
	for (FSubobjectDataHandle subobjectDataHandle : outSubobjectDataHandles)
	{
		//Avoid re-treating already treated components
		if(iteratedHandles.Contains(subobjectDataHandle)) continue;
		iteratedHandles.Add(subobjectDataHandle);
		
		auto subobjectData = subobjectDataHandle.GetData();
		if (!subobjectData->IsSceneComponent()) continue;
		
		FString componentName = subobjectData->GetAssetName().ToString();
		if (componentName.Left(5) != "MIMIC")continue;
		
		beaconsHandles.Add(subobjectDataHandle);
		//We don't want to deal with end beacons first
		if (!IsStartBeacon(componentName)) continue;

		FString linkedChunkName = ExtractLinkedFurnitureChunkPart(componentName);
		FSubobjectDataHandle parentHandle = subobjectData->GetParentHandle();

		FString complementaryBeaconName = FindComplementaryBeacon(componentName);
		if (!componentsByName.Contains(complementaryBeaconName))
		{
			UE_LOG(LogTemp, Error, TEXT("%s's name refers to a part named %s, but it doesnt exist"), *componentName,
				   *complementaryBeaconName);
			continue;
		}

		auto complementaryBeaconHandle = componentsByName[complementaryBeaconName];
		auto complementaryBeaconParentHandle = complementaryBeaconHandle.GetData()->GetParentHandle();

		//We create a new scene component for the complementary beacon
		auto complementaryBeaconSceneComponent=Cast<USceneComponent>(complementaryBeaconHandle.GetData()->GetObject());
		UAttachPoint* complementaryBeaconComponent=NewObject<UAttachPoint>();
		complementaryBeaconComponent->CreationMethod=EComponentCreationMethod::UserConstructionScript;
		complementaryBeaconComponent->SetMobility(EComponentMobility::Movable);
		complementaryBeaconComponent->SetRelativeTransform(complementaryBeaconSceneComponent->GetRelativeTransform());
		FString complementaryBeaconComponentName="End_"+linkedChunkName+"_TO_"+complementaryBeaconParentHandle.GetData()->GetAssetName().ToString();
		complementaryBeaconComponent->Rename(*complementaryBeaconComponentName);

		//And add it using the subsystem
		FAddNewSubobjectParams params;
		params.ParentHandle = complementaryBeaconParentHandle;
		params.NewClass = UAttachPoint::StaticClass();
		params.BlueprintContext = Blueprint;
		params.AssetOverride = complementaryBeaconComponent;
		FText failureReason = FText::FromString(TEXT(""));
		subobjectSubsystem->AddNewSubobject(params, failureReason);
		if (!failureReason.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Could not add furniture joint: %s"), *failureReason.ToString());
		}
		
		//We create the new furniture joint
		auto staticMeshComponent = Cast<UStaticMeshComponent>(subobjectData->GetObject());
		UFurnitureJoint* furnitureJoint = NewObject<UFurnitureJoint>();
		furnitureJoint->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		furnitureJoint->SetMobility(EComponentMobility::Movable);
		furnitureJoint->SetRelativeTransform(staticMeshComponent->GetRelativeTransform());
		furnitureJoint->EndAttachPointName=complementaryBeaconComponentName;
		FString furnitureJointName="Joint_"+linkedChunkName;
		furnitureJoint->Rename(*furnitureJointName);

		//And add it using the subsystem
		params.ParentHandle = parentHandle;
		params.NewClass = UFurnitureJoint::StaticClass();
		params.BlueprintContext = Blueprint;
		params.AssetOverride = furnitureJoint;
		failureReason = FText::FromString(TEXT(""));
		subobjectSubsystem->AddNewSubobject(params, failureReason);
		if (!failureReason.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Could not add furniture joint: %s"), *failureReason.ToString());
		}
	}
	subobjectSubsystem->DeleteSubobjects(contextHandle, beaconsHandles, Blueprint);
}
