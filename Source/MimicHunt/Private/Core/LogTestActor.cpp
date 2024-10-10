#include "Core/LogTestActor.h"

#include "Utils/LLog.h"

LL_FILE_CVAR(LogTestActor);


// Sets default values
ALogTestActor::ALogTestActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LL_DBG(this, "ALogTestActor::ALogTestActor : Constructor");
}

// Called when the game starts or when spawned
void ALogTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALogTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

