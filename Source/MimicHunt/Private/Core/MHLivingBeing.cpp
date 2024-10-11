#include "Core/MHLivingBeing.h"


AMHLivingBeing::AMHLivingBeing()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMHLivingBeing::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMHLivingBeing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMHLivingBeing::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

