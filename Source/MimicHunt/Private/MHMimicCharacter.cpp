#include "MHMimicCharacter.h"


AMHMimicCharacter::AMHMimicCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMHMimicCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMHMimicCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMHMimicCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

