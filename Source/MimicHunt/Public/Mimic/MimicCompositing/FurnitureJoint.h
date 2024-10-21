// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrganBundle.h"
#include "Components/SceneComponent.h"
#include "Mimic/Mimic.h"
#include "FurnitureJoint.generated.h"

//This class represent the joint between two parts of a furniture piece, where you can add mimic eyes, legs etc.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MIMICHUNT_API UFurnitureJoint : public UChildActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFurnitureJoint();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	void OnMimicBirth();
	void OnMimicWake();
	void OnMimicSleep();

	UPROPERTY(BlueprintReadOnly)
	AMimic* Mimic;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Furniture Joint")
	TArray<UOrganBundle*> OrganBundles;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Furniture Joint")
	bool HideParentOnMimicWake=false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Furniture Joint")
	bool HideChildOnMimicWake=false;
	//What furniture chunk is the parent of this joint, name should be automatically set by the Script Asset Action and not modified by the GD
	UPROPERTY(EditAnywhere, Category="Furniture Joint|Generated by editor scripting")
	FString ParentChunkName;
	//What furniture chunk is the parent of this joint, name should be automatically set by the Script Asset Action and not modified by the GD
	UPROPERTY(EditAnywhere, Category="Furniture Joint|Generated by editor scripting")
	FString ChildChunkName;
	UPROPERTY(EditAnywhere, Category="Furniture Joint|Generated by editor scripting")
	FTransform SecondAttachPointTransform;

	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* ParentChunkComponent=nullptr;
	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* ChildChunkComponent=nullptr;
	UPROPERTY()
	UStaticMesh* ParentChunkMesh=nullptr;
	UPROPERTY()
	UStaticMesh* ChildChunkMesh=nullptr;

	UPROPERTY()
	AMimicOrgan* Organ=nullptr;
};
