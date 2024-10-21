// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/MimicLeg/RigCustomNodesLibrary.h"

FRigUnit_RotateLeg_Execute()
{
	FVector2d footPosition=FVector2d(PreviousFootTarget.X, PreviousFootTarget.Y);
	FVector2d legAttachment=FVector2d(LegAttachmentPosition.X, LegAttachmentPosition.Y);
	FVector2d legToFoot=footPosition-legAttachment;
	float angleAsRadians=FMath::DegreesToRadians(Angle);
	float rotatedX=legToFoot.X*FMath::Cos(angleAsRadians)-legToFoot.Y*FMath::Sin(angleAsRadians);
	float rotatedY=legToFoot.X*FMath::Sin(angleAsRadians)+legToFoot.Y*FMath::Cos(angleAsRadians);

	NewFootTarget=FVector(LegAttachmentPosition.X+rotatedX,LegAttachmentPosition.Y+rotatedY,PreviousFootTarget.Z);
}

FRigUnit_NewFootTarget_Execute()
{
	//If the movement is purely on the Z axis (up down) it's pointless to move the foot
	if(abs(Direction.X)<0.001 && abs(Direction.Y)<0.001)
	{
		NewFootTarget=PreviousFootTarget;
		return;
	};
	
	//Previously, the size of the step was based on how much the foot was behind but it was scrapped
	// FVector3f diff=PreviousFootTarget-LegAttachmentPosition;
	// diff.Z=0;
	// float behindnessFactor=FVector3f::DotProduct(diff,Direction);
	FVector legAttachmentToFoot=PreviousFootTarget-LegAttachmentPosition;
	FVector perpendicularToDirection=FVector(Direction.Y,-Direction.X,0);
	
	//We get the vector part perpendicular to direction because this way, we can keep
	//this part the same after the step
	float dotAlongPerpendicularToDirection=FVector::DotProduct((legAttachmentToFoot),perpendicularToDirection);

	//If the feet got really far the dot along perpendicular direction can get huge, so we bring it back to a human level
	if(dotAlongPerpendicularToDirection*dotAlongPerpendicularToDirection>MaxDistanceSqr)
	{
		dotAlongPerpendicularToDirection=0.5f*sqrt(MaxDistanceSqr);
	}
	//We need to compute the new target position so that if you stop moving now, and foot move, the leg will be as stretched as it can be.
	//Meaning the vector LegAttachment-NewFootTarget should have a norm equal to the max distance
	float remainingDistanceSqr=MaxDistanceSqr-dotAlongPerpendicularToDirection*dotAlongPerpendicularToDirection-legAttachmentToFoot.Z*legAttachmentToFoot.Z;
	if(remainingDistanceSqr<0)
	{
		NewFootTarget=PreviousFootTarget;
		return;
	};

	NewFootTarget=LegAttachmentPosition+perpendicularToDirection*dotAlongPerpendicularToDirection+Direction*sqrt(remainingDistanceSqr);
	NewFootTarget.Z=PreviousFootTarget.Z;

	MaxLiftFootPosition=LegAttachmentPosition+perpendicularToDirection*dotAlongPerpendicularToDirection;
	MaxLiftFootPosition.Z=PreviousFootTarget.Z;
}
