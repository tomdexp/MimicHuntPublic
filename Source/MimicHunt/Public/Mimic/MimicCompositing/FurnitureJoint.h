// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FurnitureJoint.generated.h"

UENUM(BlueprintType)
enum class ESpawnableOrgans : uint8
{
	None        = 0 UMETA(DisplayName = "None"),
	Leg     = 1 << 0 UMETA(DisplayName = "Leg"),
	Eye     = 1 << 1 UMETA(DisplayName = "Eye"),
};
ENUM_CLASS_FLAGS(ESpawnableOrgans)

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

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ESpawnableOrgans SpawnableOrgans;
	//What furniture chunk is the parent of this joint, name should be automaticly set by the Script Asset Action and not modified by the GD
	UPROPERTY(EditAnywhere)
	FString ParentChunkName;
	//What furniture chunk is the parent of this joint, name should be automaticly set by the Script Asset Action and not modified by the GD
	UPROPERTY(EditAnywhere)
	FString ChildChunkName;
	UPROPERTY(EditAnywhere)
	FTransform SecondAttachPointTransform;

	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* ParentChunkComponent;
	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* ChildChunkComponent;
};
