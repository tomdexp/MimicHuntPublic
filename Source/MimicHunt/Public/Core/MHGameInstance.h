#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MHGameInstance.generated.h"

UCLASS()
class MIMICHUNT_API UMHGameInstance : public UGameInstance
{
	GENERATED_BODY()
	virtual void Init() override;

protected:
	void AutoLogin();
};
