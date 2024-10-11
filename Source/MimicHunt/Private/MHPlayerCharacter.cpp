#include "MHPlayerCharacter.h"


AMHPlayerCharacter::AMHPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMHPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMHPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMHPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

