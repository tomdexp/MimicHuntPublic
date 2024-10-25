// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/FurnitureMimicMap.h"

UBlueprintGeneratedClass* UFurnitureMimicMap::GetFurniture(UBlueprintGeneratedClass* mimic)
{
	for (auto entry : Entries)
	{
		if(entry.Mimic == mimic) return entry.Furniture;
	}
	return nullptr;
}

UBlueprintGeneratedClass* UFurnitureMimicMap::GetMimic(UBlueprintGeneratedClass* furniture)
{
	for (auto entry : Entries)
	{
		if(entry.Furniture == furniture) return entry.Mimic;
	}
	return nullptr;
}
