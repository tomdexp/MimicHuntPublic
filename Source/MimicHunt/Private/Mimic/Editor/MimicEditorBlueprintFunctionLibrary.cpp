// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/Editor/MimicEditorBlueprintFunctionLibrary.h"
#include "Mimic/Mimic.h"
#include "Utils/LLog.h"
LL_FILE_CVAR(LogMimicEditor);

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

	AMimic* mimic=Cast<AMimic>(DefaultObject);

	USubobjectDataSubsystem* subobjectSubsystem = GEditor->GetEngineSubsystem<USubobjectDataSubsystem>();
   
	if (!subobjectSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("SubobjectDataSubsystem not found."));
		return;
	}

	// TArray<FSubobjectDataHandle> outSubobjectData;
	// subobjectSubsystem->GatherSubobjectData(DefaultObject, outSubobjectData);
	// for (FSubobjectDataHandle subobjectData : outSubobjectData)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Subobject data found. %s"),subobjectData->);
	// }
	
	//  TSharedPtr<FSubobjectDataHandle> RootHandle = SubobjectSubsystem->FindHandleForObject(mimic);
	//
	// if (!RootHandle.IsValid())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Impossible de trouver les sous-objets pour l'acteur : %s"), *mimic->GetName());
	// 	return;
	// }
}
