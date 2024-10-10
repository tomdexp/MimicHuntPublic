// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/RigUnit.h"
#include "RigUnit_TargetFootNode.generated.h"

/**
 * 
 */
USTRUCT(meta = (DisplayName = "My Custom Node", Category = "Custom"))
struct MIMICHUNT_API FRigUnit_TargetFootNode : public FRigUnit
{
	GENERATED_BODY()
	
    FRigUnit_TargetFootNode()
        : A(0.f), B(0.f), Result(0.f) {}

    RIGVM_METHOD()
        virtual void Execute() override;

    UPROPERTY(meta = (Input))
    float A;

    UPROPERTY(meta = (Input))
    float B;

    UPROPERTY(meta = (Output))
    float Result;

};
