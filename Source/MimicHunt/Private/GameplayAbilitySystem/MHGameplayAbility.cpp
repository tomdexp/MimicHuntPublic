#include "GameplayAbilitySystem/MHGameplayAbility.h"

UMHGameplayAbility::UMHGameplayAbility()
{
	// See : https://github.com/tranek/GASDocumentation?tab=readme-ov-file#4612-server-respects-remote-ability-cancellation
	bServerRespectsRemoteAbilityCancellation = false;
}
