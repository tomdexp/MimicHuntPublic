// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AttachPoint.generated.h"


//Used to connect organs to chunks, chunks to chunks etc. When you connect 2 of these elements, two of their attach point should touch
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MIMICHUNT_API UAttachPoint : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAttachPoint();
	//Is the start attachment point
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsStart=false;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	static void PlaceChildRelativeToParent(USceneComponent* Child,USceneComponent* ChildAttachPoint,const USceneComponent* Parent, const USceneComponent* ParentAttachPoint, bool RotateChild=false);
};
