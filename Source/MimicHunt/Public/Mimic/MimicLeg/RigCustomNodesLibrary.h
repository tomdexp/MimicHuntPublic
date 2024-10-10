// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Units/RigUnit.h"
#include "RigCustomNodesLibrary.generated.h"

USTRUCT(meta = (DisplayName = "New foot target", Category = "Custom"))
struct MIMICHUNT_API FRigUnit_NewFootTarget: public FRigUnit {
	GENERATED_BODY()

	FRigUnit_NewFootTarget()
		: MaxDistanceSqr(0), PreviousFootTarget(0), LegAttachmentPosition(0), Direction(0), NewFootTarget(0), MaxLiftFootPosition(0) {}

	RIGVM_METHOD()
	virtual void Execute() override;

	UPROPERTY(meta = (Input))
	float MaxDistanceSqr;
	
	UPROPERTY(meta = (Input))
	FVector PreviousFootTarget;

	UPROPERTY(meta = (Input))
	FVector LegAttachmentPosition;

	UPROPERTY(meta = (Input))
	FVector Direction;

	UPROPERTY(meta = (Output))
	FVector NewFootTarget;

	//The position at which the foot is lifted the highest in a step
	UPROPERTY(meta = (Output))
	FVector MaxLiftFootPosition;
};

UCLASS()
class MIMICHUNT_API URigCustomNodesLibrary : public UObject
{
	GENERATED_BODY()
};
