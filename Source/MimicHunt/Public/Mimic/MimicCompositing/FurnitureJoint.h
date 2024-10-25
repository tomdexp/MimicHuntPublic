// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrganBundle.h"
#include "Components/SceneComponent.h"
#include "Mimic/Mimic.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
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
	FTransform _childChunkCachedRelativeTransform;
	USceneComponent* _childChunkCachedParent;
	UPhysicsConstraintComponent* _startConstraintComponent;
	UPhysicsConstraintComponent* _endConstraintComponent;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	void OnMimicBirth();
	void OnMimicWake();
	void OnMimicSleep();

	UPROPERTY(BlueprintReadOnly)
	AMimic* Mimic;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="FurnitureJoint")
	TArray<UOrganBundle*> OrganBundles;
	//Flip the start and end
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="FurnitureJoint")
	bool IsFlipped;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="FurnitureJoint")
	bool HideParentOnMimicWake=false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="FurnitureJoint")
	bool HideChildOnMimicWake=false;
	UPROPERTY(EditAnywhere, Category="FurnitureJoint|Generated by editor scripting")
	FString EndAttachPointName;

	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* ParentChunkComponent=nullptr;
	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* ChildChunkComponent=nullptr;
	UPROPERTY()
	UStaticMesh* ParentChunkMesh=nullptr;
	UPROPERTY()
	UStaticMesh* ChildChunkMesh=nullptr;
	UPROPERTY()
	USceneComponent* EndAttachPoint=nullptr;

	FVector StartToEndVector=FVector::ZeroVector;

	UPROPERTY()
	AMimicOrgan* Organ=nullptr;
};
