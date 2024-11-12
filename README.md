![Unreal Engine](https://img.shields.io/badge/unrealengine-%23313131.svg?style=for-the-badge&logo=unrealengine&logoColor=white)
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Jira](https://img.shields.io/badge/jira-%230A0FFF.svg?style=for-the-badge&logo=jira&logoColor=white)
![Confluence](https://img.shields.io/badge/confluence-%23172BF4.svg?style=for-the-badge&logo=confluence&logoColor=white)

![GitHub commit activity](https://img.shields.io/github/commit-activity/t/tomdexp/MimicHuntPublic?authorFilter=tomdexp&style=for-the-badge)

## Mimic Hunt
## Table of contents
- [What is this repository ?](#what-is-this-repository-)
- [General](#general)
    - [The pitch](#the-pitch)
    - [Context](#context)
    - [Credits](#credits)
    - [Mimic Hunt Gameplay Framework Architecture](#mimic-hunt-gameplay-framework-architecture)
    - [Features](#features)
        - [Interaction system using the Gameplay Ability System](#interaction-system-using-the-gameplay-ability-system)
            - [Overview of the interaction system](#overview-of-the-interaction-system)
            - [UInteractableComponent](#uinteractablecomponent)
            - [GA_Interact](#ga_interact)
            - [UAbilityTask_GrantNearbyInteraction](#uabilitytask_grantnearbyinteraction)
            - [UAbilityTask_WaitForInteractableComponentSingleLineTrace](#uabilitytask_waitforinteractablecomponentsinglelinetrace)
            - [The interaction system inside the engine](#the-interaction-system-inside-the-engine)
            - [Interactable Actors](#interactable-actors)
    - [Usage of coroutines in the project](#usage-of-coroutines-in-the-project)
        - [Using coroutine to work with designers](#using-coroutine-to-work-with-designers)
            - [WaitForPlayerState with Unreal Latent Action](#waitforplayerstate-with-unreal-latent-action)
            - [WaitForPlayerState with UE5Coro plugin](#waitforplayerstate-with-ue5coro-plugin)
        - [Using coroutine to solve race conditions](#using-coroutine-to-solve-race-conditions)
## What is this repository ?
This repository is the public version of the source code of my Master 2 Game (only the first milestone, the game is still in progress) : Mimic Hunt.
It only contains the source code, so it won't compile.

## General
### The pitch
Mimic Hunt is a social Horror Game for 2-4 players, along the lines of Lethal Company and Phasmophobia. 
However, this time you are the hunter and not the hunted. During a Mimic pandemic, nasty beasts that hide by imitating furniture, 
you try to gain reputation as the best exterminator in town. 
But be careful, it is a dangerous job.

### Context
As of the 1st milestone (end of October 2024), this game is a vertical slice that I am working on during my Master's degree 2nd year project.
This project started in September 2024 and will continue until February 2025.
We are a multidisciplinary team of 10 people. 

As the the online programmer of this game, I am working on
- Ensuring correct and efficient networked gameplay by leveraging Unreal Engine's Networking
- Implementing the Gameplay Ability System according to the game requirements
- Using the Epic Online Services to enable lobby creation and joining
- Working with the Sound Designer to implement Voice Chat using Wwise and ODIN's Voice Chat

### Credits
Timothée BOLLA, Producer

Franziska BLANK, Game/Combat Designer

Léo Zinani DEBAECKER, Game/Level Designer 

Tom D’EXPORT, Online Programmer _(that's me)_

Jérémy LOÏC AUCLAIR, Programmer 

Laura MOGET, Environment Artist

Florian CARON, Game Artist

Simon RUIZ, UX/UI Designer

Alexandre AGNIEL-ROUX, UX & UR

Frédéric KUKOVICIC, Sound Design

### Mimic Hunt Gameplay Framework Architecture
Mimic Hunt is structured in two distinct game modes : Lobby and Combat.
Early in the project, I decided to create a diagram of our game architecture with the gameplay framework that Unreal Engine provides.

![Mimic Hunt Gameplay Framework Architecture](Content/mimichunt-gameplay-framework-architecture.png)

### Features
1. An interaction system using the Gameplay Ability System
2. Voice chat integration with ODIN

(I am only listing the features that I worked on, there is a procedural Mimic Generation system but I did not do it, so it won't be detailed here)


#### Interaction system using the Gameplay Ability System
##### Overview of the interaction system
The interaction system we have works with GAS to leverage the prediction and cancellation features of the Gameplay Ability System, 
here is a quick overview of the system.  
It is inspired by the interaction system from Lyra (https://dev.epicgames.com/documentation/en-us/unreal-engine/lyra-sample-game-interaction-system-in-unreal-engine) but much simpler 
(which means fewer features, especially we can't detect multiples interactables in the same line trace, we assume the player will only ever have one active interactable to use in 
front of him in our Game).
##### UInteractableComponent
* Derives from `UActorComponent`
* Has a `FText InteractionText` to show the UI for the interaction
* Has a `bool bIsInteractable` to enable or disable the ability for the component to be interacted with
* Has a `TSubclassOf<UGameplayAbility> InteractionAbility` that is granted by the `UAbilityTask_GrantNearbyInteraction`
* Has a `TWeakObjectPtr<UAbilitySystemComponent> TargetAbilitySystem` If we want to trigger the ability on a different target than the player

##### GA_Interact
* Blueprint derived from `MHGameplayAbility_Interact` C++ class in `Source/MimicHunt/Public/GameplayAbilitySystem/Interactions/MHGameplayAbility_Interact.h`
* Is automatically activated when granted when the player character spawns
* Automatically activate the `UAbilityTask_GrantNearbyInteraction` C++ class
* Can trigger interaction with the `UMHGameplayAbility_Interact::TriggerInteraction()` method in the `MHGameplayAbility_Interact` class

##### UAbilityTask_GrantNearbyInteraction
* Derives from `UAbilityTask`
* Is only **executed** on the **server**
* Detects `UInteractableComponent` in a sphere radius around the player, when the component is detected, its `InteractionAbility` is given to the player by the server
* Has a `float InteractionScanRange = 500` that represent the size of the sphere of detection around the player in cm
* Has a `float InteractionScanRate = 0.1` that represent the delay between each scan, for example, with 0.1, it will scan every 0,1 second
* Has a `TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache` to gather all granted abilities in the radius

https://github.com/user-attachments/assets/e0ce93ce-7a91-4f5c-9cd5-af294c7cd4b1
> The green sphere is the `InteractionScanRange` of the `UAbilityTask_GrantNearbyInteraction`, when an interactable component is detected on an actor, the associated `InteractionAbility` is granted to the player before he can interact with it to compensate the delay when the server grants the ability

##### UAbilityTask_WaitForInteractableComponentSingleLineTrace
* Derives from `UAbilityTask`
* Has a float `InteractionScanRange = 100` that represent the length of the line trace of detection in front of the player in cm (the scan range of this should always be inferior to the one of the `UAbilityTask_GrantNearbyInteraction` to compensate for the delay between when the server grants the ability and when the player can really interact with the object)
* Has a float `InteractionScanRate = 0.1` that represent the delay between each scan, for example, with 0.1, it will scan every 0,1 second
* Has a `DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableObjectsChangedEvent, UInteractableComponent*, InteractableComponent)` that is used inside the `GA_Interact` to update the current interactable

https://github.com/user-attachments/assets/2196abe2-fc0c-4ad8-8dd2-75466f97e4bb
> Video of the `UAbilityTask_WaitForInteractableComponentSingleLineTrace` scanning for collisions, as you can notice, it has a shorter range than the green sphere.


##### The interaction system inside the engine
![GA_Interact - Activate/End Ability](Content/mimichunt-ga-interact-activate-end-ability.png)
> The Event ActivateAbility is automatically triggered when the Ability is given because we have a bool bActivateOnSpawn for our MHGameplayAbility
---
![GA_Interact - Look For Actors](Content/mimichunt-ga-interact-look-for-actors.png)
> This uses the UAbilityTask_WaitForInteractableComponentSingleLineTrace and Update the current interactable when the interactable object changes
---
![GA_Interact - Press And Release](Content/mimichunt-ga-interact-press-release.png)
> We wait for the gameplay event Event.Ability.Interaction.Pressed that is sent when the player press the interaction inputs inside. It executes the TriggerInteraction method of the MHGameplayAbility_Interact

##### Interactable Actors
Then for the interactable actor setup itself, let’s take a look at how the exit door interaction is implemented.
In Mimic Hunt, the "Exit Door" represent the system that allow players to quit the current hunt, 
it is composed of a door that can be opened by a player and a zone that is activated after,
for players to leave the map they have to stay inside the zone for 5 seconds.

![BP_ExitDoor - Hierarchy](Content/mimichunt-exit-door-hierarchy.png)
> The `BP_ExitDoor` has an InteractableComponent (see setup below)
---
![BP_ExitDoor - Interactable Component Setup](Content/mimichunt-exit-door-interactable-component.png)
> We have created a specific gameplay ability for this interaction called `GA_Interaction_OpenExitDoor` (see below)
---

![GA_Interaction_OpenExitDoor - Settings](Content/mimichunt-exit-door-trigger-gameplay-event.png)
> For the ability to be activated, we setup the ability so that is trigger with gameplay event, for every interaction with a gameplay event called `Event.Ability.Interaction.Activate`
---

![GA_Interaction_OpenExitDoor - Activate Ability](Content/mimichunt-exit-door-trigger-gameplay-ability-open-exit-door.png)
> We use the `Event ActivateAbilityFromEvent` instead of the classic `Event ActivateAbility`, here we also deactivate the ability to interact with the door after the 1st time and trigger the Open Door event on the `BP_ExitDoor`
---
### Usage of coroutines in the project
This project is using the [UE5Coro plugin](https://github.com/landelare/ue5coro) to implements C++20 coroutine inside Unreal Engine 5.
Coroutine have been wonderful for this project for two main things
- It enables the sound designer to work on the voice chat integration without worrying about initialization order
(e.g. I added a lot of utility methods like WaitForPlayerState or WaitForPlayerController to use in blueprints)
- It enables me to solves race conditions easily (example below)

#### Using coroutine to work with designers
When Frederic (the sound designer) and I started to work on the voice chat integration,
He explored the voice chat example project provided by ODIN in order to integrate it by himself as most as possible in our project.
Then, when integrating into our project he faced a lot of race conditions 
(e.g. player state being not set on remote clients when the method was trying to access it, but working on the host).
Sure I could explain him that he should instead use OnRep_PlayerState() for every remote clients and use PossessedBy() for the server, 
but since he was only using blueprint, I would have to teach him overriding methods in C++ and triggering event from C++ for blueprints.

OR, I could quickly create him a latent blueprint node called "WaitForPlayerState" that stops its execution until the PlayerState
is valid and works the same way for host and remote client.

Creating Latent Nodes in Unreal Engine is possible, but it requires a lot of boilerplate code, 
let's see the difference between using Unreal's base latent node creation and the coroutine equivalent with [UE5Coro plugin](https://github.com/landelare/ue5coro)

##### WaitForPlayerState with Unreal Latent Action
This latent action setup is inspired from this article [Easy C++ Latent Functions in Unreal Engine 5 Blueprints](https://mikelis.net/easy-c-latent-functions-in-unreal-engine-blueprints/)

I will just paste here the header and cpp file

```c++
// WaitForPlayerStateAsyncAction.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "WaitForPlayerStateAsyncAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitForPlayerStateAsyncActionEvent);

/**
* An async action that waits until GetPlayerState() returns a valid pointer.
  */
  UCLASS()
  class PROJECT_API UWaitForPlayerStateAsyncAction : public UCancellableAsyncAction
  {
  GENERATED_BODY()

public:
/**
* Waits for the Player State to be valid.
* @param Character		The character to check for Player State.
* @param WorldContext	Object from which the world will be derived.
*/
UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext", BlueprintInternalUseOnly = "true"))
static UWaitForPlayerStateAsyncAction* WaitForPlayerState(const UObject* WorldContext, ACharacter* Character);

	/** A delegate called when the async action completes. */
	UPROPERTY(BlueprintAssignable)
	FWaitForPlayerStateAsyncActionEvent OnComplete;

	/** A delegate called when the async action fails. */
	UPROPERTY(BlueprintAssignable)
	FWaitForPlayerStateAsyncActionEvent OnFail;

	// Start UCancellableAsyncAction Functions
	virtual void Activate() override;
	virtual void Cancel() override;
	// End UCancellableAsyncAction Functions

	// Start UObject Functions
	virtual UWorld* GetWorld() const override
	{
		return ContextWorld.IsValid() ? ContextWorld.Get() : nullptr;
	}
	// End UObject Functions

private:
/** The context world of this action. */
TWeakObjectPtr<UWorld> ContextWorld = nullptr;

	/** The character to check for Player State. */
	TWeakObjectPtr<ACharacter> TargetCharacter = nullptr;

	/** The timer handle. */
	FTimerHandle CheckTimerHandle;
};
```

```c++
// WaitForPlayerStateAsyncAction.cpp

#include "WaitForPlayerStateAsyncAction.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"

UWaitForPlayerStateAsyncAction* UWaitForPlayerStateAsyncAction::WaitForPlayerState(const UObject* WorldContext, ACharacter* Character)
{
	// Ensure the character is valid
	if (!IsValid(Character))
	{
		UE_LOG(LogTemp, Warning, TEXT("WaitForPlayerState: Character is invalid."));
		return nullptr;
	}

	// Get the context world
	UWorld* ContextWorld = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
	if (!ContextWorld)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaitForPlayerState: Invalid WorldContext."));
		return nullptr;
	}

	// Create a new UWaitForPlayerStateAsyncAction, and store function arguments in it.
	UWaitForPlayerStateAsyncAction* NewAction = NewObject<UWaitForPlayerStateAsyncAction>();
	NewAction->ContextWorld = ContextWorld;
	NewAction->TargetCharacter = Character;
	NewAction->RegisterWithGameInstance(ContextWorld->GetGameInstance());

	return NewAction;
}

void UWaitForPlayerStateAsyncAction::Activate()
{
	// When the async action is ready to activate, start a timer to periodically check the Player State.
	if (UWorld* World = GetWorld())
	{
		// The timer manager is a singleton, and GetTimerManager() accessor will always return a valid one.
		FTimerManager& TimerManager = World->GetTimerManager();

		// Set a timer to check every tick (or every small interval)
		TimerManager.SetTimer(CheckTimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				if (IsActive())
				{
					if (TargetCharacter.IsValid())
					{
						if (TargetCharacter->GetPlayerState())
						{
							// If PlayerState is valid, broadcast OnComplete and cancel the action
							OnComplete.Broadcast();
							Cancel();
						}
					}
					else
					{
						// TargetCharacter is no longer valid, fail the action
						OnFail.Broadcast();
						Cancel();
					}
				}
			}),
			0.1f, true); // Check every 0.1 seconds

		return;
	}

	// If something failed, we can broadcast OnFail, and then wrap up.
	OnFail.Broadcast();
	Cancel();
}

void UWaitForPlayerStateAsyncAction::Cancel()
{
	Super::Cancel();

	// Cancel the timer
	if (CheckTimerHandle.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			FTimerManager& TimerManager = World->GetTimerManager();
			TimerManager.ClearTimer(CheckTimerHandle);
		}
	}
}
```

##### WaitForPlayerState with [UE5Coro plugin](https://github.com/landelare/ue5coro)
Let's see the exact same feature, but using coroutines.

```c++
// MHPlayerCharacter.h

#include "UE5Coro.h"

UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = LatentInfo))
FVoidCoroutine WaitForPlayerState(FLatentActionInfo LatentInfo);
```

```c++
// MHPlayerCharacter.cpp

FVoidCoroutine AMHPlayerCharacter::WaitForPlayerState(FLatentActionInfo LatentInfo)
{
    // Check if GetPlayerState() return a valid PlayerState
    while (!GetPlayerState()) 
    {
        co_await UE5Coro::Latent::NextTick(); // If not, await until next tick
    }
    // When GetPlayerState() return a valid PlayerState this will continue the blueprint execution
    co_return; 
}
```

As you can see, using Coroutines keeps the code way smaller and readable but keeps the exact same behavior.

![Coroutine usage example](Content/mimichunt-coroutine-examples.png)
> Screenshot from the BP_VoiceChat blueprint that the sound designer and I worked on,
> we can see the usage of multiple latent nodes that are all coroutines in C++


#### Using coroutine to solve race conditions
Using coroutines to create latent nodes is great, but it can also be used in C++ to easily solve race conditions.

Here is an example : when working on the interaction system, I had to implement `ActivateOnSpawn` abilities that are
automatically activated when the gameplay ability is given to the ability system component. 
However, I faced a really annoying race condition, I override the `OnGiveAbility` method of the `UGameplayAbility` to
activate the ability when it is given to the player.
However, the AvatarActor from the `FGameplayAbilityActorInfo* ActorInfo` was not set for remote clients when the ability
was given (our Ability System Component lives on the PlayerState).
My solution was to create a coroutine OnGiveAbilityCoroutine that wait for the AvatarActor to be of type Pawn, because
it was valid, but it was of type PlayerState for remote clients when the ability was given.
In the end it was a matter of 2 or 4 ticks before the AvatarActor was of type Pawn.

```c++
// Since our GAS setup has the owner actor of the ASC to the PlayerState and the Avatar to the Character
// Some clients get the OnGiveAbility before having the correct AvatarActor set.
// So we make a coroutine that wait for the AvatarActor to be of type Pawn before trying to activate the ability.
UE5Coro::TCoroutine<> UMHGameplayAbility::OnGiveAbilityCoroutine(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec)
{	
	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);	
	// Try to activate if activation policy is on spawn.
	if (ActorInfo && !Spec.IsActive() && !bIsPredicting && Cast<UMHGameplayAbility>(Spec.Ability)->bActivateOnSpawn)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();
		
		// THIS WILL WAIT FOR THE AVATAR ACTOR TO BE OF TYPE PAWN, USUALLY IT TAKES 2 OR 4 TICKS
		while (!Cast<APawn>(AvatarActor))
		{
			AvatarActor = ActorInfo->AvatarActor.Get();
			co_await UE5Coro::Latent::NextTick();
		}
		// ...(abbreviated for brevity)
	co_return;
}
```