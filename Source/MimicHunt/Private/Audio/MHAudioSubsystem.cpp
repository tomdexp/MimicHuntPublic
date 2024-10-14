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

void UMHAudioSubsystem::SetVoiceChatUserID(const int32 UserID)
{
	LL_DBG(this, "SetVoiceChatUserID called with UserID: %s", UserID);
	FTencentGMEDevice::SetUserID(FString::FromInt(UserID));
}

void UMHAudioSubsystem::SetVoiceChatRoomID(const FString RoomID)
{
	LL_DBG(this, "SetVoiceChatRoomID called with RoomID: %s", RoomID);
	FTencentGMEDevice::SetRoomID(RoomID);
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