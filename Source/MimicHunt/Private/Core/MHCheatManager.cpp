#include "Core/MHCheatManager.h"

#include "MHPlayerCharacter.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(MHCheatManager);

void UMHCheatManager::SummonFakePlayer()
{
    // FakePlayerCharacterClass is a TSubclassOf<AMHPlayerCharacter>
    SpawnActorWithLineTrace(PlayerCharacterClass);
}

void UMHCheatManager::SummonMimic()
{
    // MimicCharacterClass is a TSubclassOf<AMHMimicCharacter>
    SpawnActorWithLineTrace(MimicCharacterClass);
}

void UMHCheatManager::SpawnActorWithLineTrace(TSubclassOf<AActor> ActorClass)
{
    APlayerController* PlayerController = GetPlayerController();
    if (PlayerController)
    {
        FVector Location;
        FRotator Rotation;
        PlayerController->GetPlayerViewPoint(Location, Rotation);

        FVector End = Location + (Rotation.Vector() * 1000.0f);
        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(PlayerController->GetPawn());

        if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, End, ECC_Visibility, Params))
        {
            Location = HitResult.Location;
        }
        else
        {
            Location = End;
        }

        if (ActorClass)
        {
            FActorSpawnParameters SpawnParams;
            AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClass, Location, FRotator::ZeroRotator, SpawnParams);
        }
        else
        {
            LL_ERR(this, "PlayerCharacterClass is nullptr, please set it in the editor");
        }
    }
}
