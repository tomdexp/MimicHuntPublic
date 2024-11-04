#pragma once

#include "CoreMinimal.h"
#include "MHPlayerCharacter.h"
#include "Engine/GameInstance.h"
#include "MHGameInstance.generated.h"

class AVoiceChat;

UCLASS()
class MIMICHUNT_API UMHGameInstance : public UGameInstance
{
	GENERATED_BODY()
	virtual void Init() override;
	

protected:
	void AutoLogin();
public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TMap<FGuid, AVoiceChat*> IdsToPlayerVoiceChatActors;
	bool IsTransitionScene();
};
