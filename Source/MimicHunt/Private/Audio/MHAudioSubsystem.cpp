#include "Audio/MHAudioSubsystem.h"

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
