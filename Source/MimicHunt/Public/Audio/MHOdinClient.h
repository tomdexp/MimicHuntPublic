// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OdinRoom.h"
#include "OdinAudioCapture.h"
#include "Components/ActorComponent.h"
#include "MHOdinClient.generated.h"


UCLASS(Blueprintable)
class MIMICHUNT_API UMHOdinClient : public UActorComponent
{
	GENERATED_BODY()

public:
	UMHOdinClient();

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UOdinRoom> OdinRoomPtr;
};