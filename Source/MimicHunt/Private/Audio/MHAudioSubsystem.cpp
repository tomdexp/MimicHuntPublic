#include "Audio/MHAudioSubsystem.h"

DEFINE_LOG_CATEGORY(MHAudio);

void UMHAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(MHAudio, Log, TEXT("MimicHunt Audio Subsystem initialized"));
}

void UMHAudioSubsystem::ExampleFunction()
{
	UE_LOG(MHAudio, Log, TEXT("ExampleFunction called"));
}
