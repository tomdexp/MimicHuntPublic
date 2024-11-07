#include "Core/MHCheatManager.h"

#include "MHPlayerCharacter.h"
#include "Core/MHGameState.h"
#include "Mimic/Furniture.h"
#include "Mimic/Mimic.h"
#include "Utils/LLog.h"

class AMHGameState;
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

void UMHCheatManager::AddLobbyMoney(int32 Amount)
{
    if (AMHGameState* GameState = GetWorld()->GetGameState<AMHGameState>())
    {
        if (!GameState->HasAuthority()) return;
        GameState->PersistentDataManager->AddLobbyMoney(Amount);
    }
}

void UMHCheatManager::RemoveLobbyMoney(int32 Amount)
{
    if (AMHGameState* GameState = GetWorld()->GetGameState<AMHGameState>())
    {
        if (!GameState->HasAuthority()) return;
        GameState->PersistentDataManager->RemoveLobbyMoney(Amount);
    }
}

void UMHCheatManager::SetLobbyMoney(int32 Amount)
{
    if (AMHGameState* GameState = GetWorld()->GetGameState<AMHGameState>())
    {
        if (!GameState->HasAuthority()) return;
        GameState->PersistentDataManager->SetLobbyMoney(Amount);
    }
}

void UMHCheatManager::GetLobbyMoney()
{
    if (AMHGameState* GameState = GetWorld()->GetGameState<AMHGameState>())
    {
        if (!GameState->HasAuthority()) return;
        LL_DBG(this, "Lobby Money: {0}", GameState->PersistentDataManager->GetLobbyMoney());
    }
}

void UMHCheatManager::BirthMimic()
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

        // Set the sphere radius for the trace
        float SphereRadius = 50.0f;  // Adjust the radius as needed

        // Perform a sphere trace
        if (GetWorld()->SweepSingleByProfile(HitResult, Location, End, FQuat::Identity, "Pawn", FCollisionShape::MakeSphere(SphereRadius), Params))
        {
            // Check if we hit an actor and print its name
            if (AActor* HitActor = HitResult.GetActor())
            {
                UE_LOG(LogTemp, Log, TEXT("Hit Actor: %s"), *HitActor->GetName());

                // Attempt to cast the hit actor to AFurniture
                AFurniture* Furniture = Cast<AFurniture>(HitActor);
                if (Furniture)
                {
                    // Call the TurnToMimic function on the furniture if the cast is successful
                    Furniture->TurnToMimic();
                    UE_LOG(LogTemp, Log, TEXT("Called TurnToMimic on: %s"), *Furniture->GetName());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Hit actor is not of type AFurniture"));
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("No Actor hit in sphere trace"));
        }
    }
}

void UMHCheatManager::BirthMimicsInRadius(float Radius)
{
    APlayerController* PlayerController = GetPlayerController();
    if (PlayerController)
    {
        FVector Location;
        FRotator Rotation;
        PlayerController->GetPlayerViewPoint(Location, Rotation);

        TArray<FOverlapResult> OverlapResults;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(PlayerController->GetPawn());

        if (GetWorld()->OverlapMultiByProfile(OverlapResults, Location, FQuat::Identity, "Pawn", FCollisionShape::MakeSphere(Radius), Params))
        {
            for (const FOverlapResult& Result : OverlapResults)
            {
                if (AActor* HitActor = Result.GetActor())
                {
                    AFurniture* Furniture = Cast<AFurniture>(HitActor);
                    if (Furniture)
                    {
                        Furniture->TurnToMimic();
                        UE_LOG(LogTemp, Log, TEXT("Called TurnToMimic on: %s"), *Furniture->GetName());
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("No AFurniture actors found in radius"));
        }
    }
}

void UMHCheatManager::WakeMimic()
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

        // Set the sphere radius for the trace
        float SphereRadius = 5.0f;  // Adjust the radius as needed

        // Perform a sphere trace
        if (GetWorld()->SweepSingleByProfile(HitResult, Location, End, FQuat::Identity, "Pawn", FCollisionShape::MakeSphere(SphereRadius), Params))
        {
            // Check if we hit an actor and print its name
            if (AActor* HitActor = HitResult.GetActor())
            {
                UE_LOG(LogTemp, Log, TEXT("Hit Actor: %s"), *HitActor->GetName());

                // Attempt to cast the hit actor to AFurniture
                AMimic* Mimic = Cast<AMimic>(HitActor);
                if (Mimic)
                {
                    // Call the TurnToMimic function on the furniture if the cast is successful
                    Mimic->MimicWake();
                    UE_LOG(LogTemp, Log, TEXT("Called Wake on: %s"), *Mimic->GetName());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Hit actor is not of type AMimic"));
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("No Actor hit in sphere trace"));
        }
    }
}

void UMHCheatManager::WakeMimicsInRadius(float Radius)
{
    APlayerController* PlayerController = GetPlayerController();
    if (PlayerController)
    {
        FVector Location;
        FRotator Rotation;
        PlayerController->GetPlayerViewPoint(Location, Rotation);

        TArray<FOverlapResult> OverlapResults;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(PlayerController->GetPawn());
        if (GetWorld()->OverlapMultiByProfile(OverlapResults, Location, FQuat::Identity, "Pawn", FCollisionShape::MakeSphere(Radius), Params))
        {
            for (const FOverlapResult& Result : OverlapResults)
            {
                if (AActor* HitActor = Result.GetActor())
                {
                    AMimic* Mimic = Cast<AMimic>(HitActor);
                    if (Mimic)
                    {
                        Mimic->MimicWake();
                        UE_LOG(LogTemp, Log, TEXT("Called Wake on: %s"), *Mimic->GetName());
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Actor is not of type AMimic"));
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("No AMimic actors found in radius"));
        }
    }
}

void UMHCheatManager::SleepMimic()
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

        // Set the sphere radius for the trace
        float SphereRadius = 5.0f;  // Adjust the radius as needed

        // Perform a sphere trace
        if (GetWorld()->SweepSingleByProfile(HitResult, Location, End, FQuat::Identity, "Pawn", FCollisionShape::MakeSphere(SphereRadius), Params))
        {
            // Check if we hit an actor and print its name
            if (AActor* HitActor = HitResult.GetActor())
            {
                UE_LOG(LogTemp, Log, TEXT("Hit Actor: %s"), *HitActor->GetName());

                // Attempt to cast the hit actor to AFurniture
                AMimic* Mimic = Cast<AMimic>(HitActor);
                if (Mimic)
                {
                    // Call the TurnToMimic function on the furniture if the cast is successful
                    Mimic->MimicSleep();
                    UE_LOG(LogTemp, Log, TEXT("Called Sleep on: %s"), *Mimic->GetName());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Hit actor is not of type AMimic"));
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("No Actor hit in sphere trace"));
        }
    }
}

void UMHCheatManager::SleepMimicsInRadius(float Radius)
{
    APlayerController* PlayerController = GetPlayerController();
    if (PlayerController)
    {
        FVector Location;
        FRotator Rotation;
        PlayerController->GetPlayerViewPoint(Location, Rotation);

        TArray<FOverlapResult> OverlapResults;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(PlayerController->GetPawn());
        if (GetWorld()->OverlapMultiByProfile(OverlapResults, Location, FQuat::Identity, "Pawn", FCollisionShape::MakeSphere(Radius), Params))
        {
            for (const FOverlapResult& Result : OverlapResults)
            {
                if (AActor* HitActor = Result.GetActor())
                {
                    AMimic* Mimic = Cast<AMimic>(HitActor);
                    if (Mimic)
                    {
                        Mimic->MimicSleep();
                        UE_LOG(LogTemp, Log, TEXT("Called Sleep on: %s"), *Mimic->GetName());
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Actor is not of type AMimic"));
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("No AMimic actors found in radius"));
        }
    }
}

void UMHCheatManager::PossessCharacter()
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

        // Set the sphere radius for the trace
        float SphereRadius = 5.0f;  // Adjust the radius as needed

        // Perform a sphere trace
        if (GetWorld()->SweepSingleByProfile(HitResult, Location, End, FQuat::Identity, "Pawn", FCollisionShape::MakeSphere(SphereRadius), Params))
        {
            // Check if we hit an actor and print its name
            if (AActor* HitActor = HitResult.GetActor())
            {
                UE_LOG(LogTemp, Log, TEXT("Hit Actor: %s"), *HitActor->GetName());
                
                ACharacter* Character = Cast<ACharacter>(HitActor);
                if (Character)
                {
                    PlayerController->Possess(Character);
                    UE_LOG(LogTemp, Log, TEXT("Called Possess on: %s"), *Character->GetName());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Hit actor is not of type ACharacter"));
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("No Actor hit in sphere trace"));
        }
    }
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
