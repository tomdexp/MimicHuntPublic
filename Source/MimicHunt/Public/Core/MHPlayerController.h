#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MHPlayerController.generated.h"

class UMHPlayerData;

/**
 * This class handles all inputs to the character
 */
UCLASS()
class MIMICHUNT_API AMHPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "DATA")
	TObjectPtr<UMHPlayerData> PlayerData;

	virtual void SetupInputComponent() override;
};
