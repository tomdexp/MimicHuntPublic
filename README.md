![Unreal Engine](https://img.shields.io/badge/unrealengine-%23313131.svg?style=for-the-badge&logo=unrealengine&logoColor=white)
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Jira](https://img.shields.io/badge/jira-%230A0FFF.svg?style=for-the-badge&logo=jira&logoColor=white)
![Confluence](https://img.shields.io/badge/confluence-%23172BF4.svg?style=for-the-badge&logo=confluence&logoColor=white)

![GitHub commit activity](https://img.shields.io/github/commit-activity/t/tomdexp/MimicHuntPublic?authorFilter=tomdexp&style=for-the-badge)

## Mimic Hunt

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
Timothée Bolla, Producer

Franziska Blank, Game/Combat Designer

Léo Zinani Debaecker, Game/Level Designer

Tom D’Export, Online Programmer _(that's me)_

Jérémy Loïc Auclair, Programmer

Laura Moget, Environment Artist

Florian Caron, Game Artist

Simon Ruiz, UX/UI Designer

Alexandre Agniel-Roux, UX & UR

Frédéric Kukovicic, Sound Design

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

INSERT VIDEO HERE

##### UAbilityTask_WaitForInteractableComponentSingleLineTrace
* Derives from `UAbilityTask`
* Has a float `InteractionScanRange = 100` that represent the length of the line trace of detection in front of the player in cm (the scan range of this should always be inferior to the one of the `UAbilityTask_GrantNearbyInteraction` to compensate for the delay between when the server grants the ability and when the player can really interact with the object)
* Has a float `InteractionScanRate = 0.1` that represent the delay between each scan, for example, with 0.1, it will scan every 0,1 second
* Has a `DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableObjectsChangedEvent, UInteractableComponent*, InteractableComponent)` that is used inside the `GA_Interact` to update the current interactable

INSERT VIDEO HERE

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

![GA_Interaction_OpenExitDoor - Activate Ability](Content/mimichunt-exit-door-trigger-gameplay-ability-open-exit-door.png)
> We use the `Event ActivateAbilityFromEvent` instead of the classic `Event ActivateAbility`, here we also deactivate the ability to interact with the door after the 1st time and trigger the Open Door event on the `BP_ExitDoor`

#### Voice chat integration with ODIN
### Usage of coroutines in the project (race conditions, gameplay, etc..)