#include "Audio/MHAudioSubsystem.h"

#include "odin_sdk.h"
#include "TencentGMEDevice.h"
#include "TencentGME_Wwise.h"
#include "TencentGMEPlugin.h"
#include "Utils/LLog.h"

LL_FILE_CVAR(MHaudio);

void UMHAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LL_DBG(this, "Audio Susbsystem initialized");
}

void UMHAudioSubsystem::ExampleFunction()
{
	LL_DBG(this, "Example function called");
}

void UMHAudioSubsystem::StartRecordingWithRecognition(const FString& speechLanguage)
{
	LL_DBG(this, "Speech language = French");
	FTencentGMEDevice::StartRecordingWithRecognition(speechLanguage);
}

void UMHAudioSubsystem::GetGMEMessage(int& localUTCTime, int& messageType, int& code, FString& message1, FString& message2)
{
	FTencentGMEDevice::GetGMEMessage(localUTCTime, messageType, code, message1, message2);
	LL_DBG(this, "Message 1 : %s", message1);
}

void UMHAudioSubsystem::StartupOdin()
{
	LL_DBG(this, "UMHAudioSubsystem::StartupOdin calling odin_startup");
	odin_startup(ODIN_VERSION);
}

void UMHAudioSubsystem::PlayerOdinIDCreated(FGuid Guid)
{
	LL_DBG(this, "UMHAudioSubsystem::PlayerOdinIDCreated : PlayerOdinIDCreated : {0}", Guid.ToString());
	OdinIDs.Add(Guid);
	NumberOfConnectedPlayers++;
}

void UMHAudioSubsystem::PlayerOdinIDDestroyed(FGuid Guid)
{
	LL_DBG(this, "UMHAudioSubsystem::PlayerOdinIDDestroyed : PlayerOdinIDDestroyed : {0}", Guid.ToString());
	OdinIDs.Remove(Guid);
	NumberOfConnectedPlayers--;
}

void UMHAudioSubsystem::CleanRoomId()
{
	if (VoiceRoomId == -1)
	{
		LL_DBG(this, "Room id already cleaned");
		return;
	}
	VoiceRoomId = -1;
	LL_DBG(this, "Room id cleaned");
}
