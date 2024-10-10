// Fill out your copyright notice in the Description page of Project Settings.


#include "Mimic/MimicLeg/RigCustomNodesLibrary.h"

FRigUnit_NewFootTarget_Execute()
{
	//This first part calculate how behind the foot is relative to the direction of the mimic
	// FVector3f diff=PreviousFootTarget-LegAttachmentPosition;
	// diff.Z=0;
	// float behindnessFactor=FVector3f::DotProduct(diff,Direction);
	FVector legAttachmentToFoot=PreviousFootTarget-LegAttachmentPosition;
	FVector perpendicularToDirection=FVector(Direction.Y,-Direction.X,0);
	
	//We get the vector part perpendicular to direction because this way, we can keep
	//this part the same after the step
	float dotAlongPerpendicularToDirection=FVector::DotProduct((legAttachmentToFoot),perpendicularToDirection);

	//We need to compute the new target position so that if you stop moving now, and foot move, the leg will be as stretched as it can be.
	//Meaning the vector LegAttachment-NewFootTarget should have a norm equal to the max distance
	float remainingDistanceSqr=MaxDistanceSqr-dotAlongPerpendicularToDirection*dotAlongPerpendicularToDirection-legAttachmentToFoot.Z*legAttachmentToFoot.Z;

	NewFootTarget=LegAttachmentPosition+perpendicularToDirection*dotAlongPerpendicularToDirection+Direction*sqrt(remainingDistanceSqr);
	NewFootTarget.Z=PreviousFootTarget.Z;

	MaxLiftFootPosition=LegAttachmentPosition+perpendicularToDirection*dotAlongPerpendicularToDirection;
	MaxLiftFootPosition.Z=PreviousFootTarget.Z;
}