#include "GZAudioMixerBridge.h"
#include "HAL/PlatformTime.h"
#include "Misc/Paths.h"

UGZAudioMixerBridge::UGZAudioMixerBridge()
{
	GroupVolumes.Add(EGZAudioChannelGroup::Master, 1.0f);
	GroupVolumes.Add(EGZAudioChannelGroup::Ambient, 0.8f);
	GroupVolumes.Add(EGZAudioChannelGroup::Weather, 0.7f);
	GroupVolumes.Add(EGZAudioChannelGroup::Vehicle, 0.85f);
	GroupVolumes.Add(EGZAudioChannelGroup::Voice, 1.0f);
}

void UGZAudioMixerBridge::Initialize()
{
	ListenerLocation = FVector::ZeroVector;
	ListenerForward = FVector::ForwardVector;
	bGlobalPaused = false;
	UE_LOG(LogTemp, Log, TEXT("[GZAudioMixerBridge] SoLoud mixer bridge initialized"));
}

void UGZAudioMixerBridge::Shutdown()
{
	StopAllSounds();
}

void UGZAudioMixerBridge::SetGroupVolume(EGZAudioChannelGroup Group, float Volume)
{
	Volume = FMath::Clamp(Volume, 0.0f, 1.0f);
	GroupVolumes.Add(Group, LogarithmicToLinear(Volume));
	UE_LOG(LogTemp, Verbose, TEXT("[GZAudioMixerBridge] Group %d volume set to %.3f"), static_cast<int32>(Group), Volume);
}

float UGZAudioMixerBridge::GetGroupVolume(EGZAudioChannelGroup Group) const
{
	return GroupVolumes.FindRef(Group);
}

void UGZAudioMixerBridge::PlayEvent(const FString& EventPath, const FZSpatialAudioParams& Params)
{
	if (EventPath.IsEmpty())
	{
		return;
	}

	float Distance = FVector::Dist(ListenerLocation, Params.Location);
	float Attenuation = ComputeAttenuation(Distance, Params.MaxDistance);
	float FinalVolume = Params.Volume * Attenuation * GetGroupVolume(EGZAudioChannelGroup::Master);
	float DopplerPitch = Params.Pitch;

	if (!Params.Velocity.IsNearlyZero())
	{
		float RelativeSpeed = FVector::DotProduct(Params.Velocity.GetSafeNormal(), (ListenerLocation - Params.Location).GetSafeNormal());
		DopplerPitch *= FMath::Clamp(1.0f + RelativeSpeed * 0.0002f, 0.85f, 1.15f);
	}

	UE_LOG(LogTemp, Verbose, TEXT("[GZAudioMixerBridge] PlayEvent %s Volume=%.3f Pitch=%.3f Distance=%.1f"), *EventPath, FinalVolume, DopplerPitch, Distance);
}

void UGZAudioMixerBridge::StopAllSounds()
{
	UE_LOG(LogTemp, Log, TEXT("[GZAudioMixerBridge] All sounds stopped"));
}

void UGZAudioMixerBridge::PauseAllSounds()
{
	bGlobalPaused = true;
	UE_LOG(LogTemp, Log, TEXT("[GZAudioMixerBridge] All sounds paused"));
}

void UGZAudioMixerBridge::ResumeAllSounds()
{
	bGlobalPaused = false;
	UE_LOG(LogTemp, Log, TEXT("[GZAudioMixerBridge] All sounds resumed"));
}

void UGZAudioMixerBridge::SetGlobalPause(bool bPaused)
{
	bGlobalPaused = bPaused;
}

void UGZAudioMixerBridge::SetListenerTransform(const FVector& Location, const FVector& Forward)
{
	ListenerLocation = Location;
	ListenerForward = Forward.GetSafeNormal();
}

float UGZAudioMixerBridge::ComputeAttenuation(float Distance, float MaxDistance) const
{
	if (Distance <= 0.0f) return 1.0f;
	if (Distance >= MaxDistance) return 0.0f;

	float t = Distance / MaxDistance;
	if (t < 0.2f)
	{
		return 1.0f - t * 1.5f;
	}
	return FMath::Pow(1.0f - t, 2.0f);
}

float UGZAudioMixerBridge::LogarithmicToLinear(float Volume01) const
{
	if (Volume01 <= 0.0f) return 0.0f;
	return FMath::Pow(Volume01, 2.0f);
}
