#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LogTestActor.generated.h"

UCLASS()
class MIMICHUNT_API ALogTestActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALogTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
