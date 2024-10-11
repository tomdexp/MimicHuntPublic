#include "Audio/MHAudioSubsystem.h"

#include "TencentGMEDevice.h"
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

void UMHAudioSubsystem::SetVoiceChatRoomID(const int32 RoomID)
{
	LL_DBG(this, "SetVoiceChatRoomID called with RoomID: %s", RoomID);
	FTencentGMEDevice::SetRoomID(FString::FromInt(RoomID));
}
