#include "Audio/SoLoudSystem/GZAudioManager.h"
#include "GuangzhouOpenWorld.h"
#include "Math/UnrealMathUtility.h"

float UGZAudioManager::GetEnvironmentReverbTime(EGZAudioEnvironment Environment)
{
	switch (Environment)
	{
	case EGZAudioEnvironment::Street:     return 1.2f;
	case EGZAudioEnvironment::QilouAlley: return 2.8f;
	case EGZAudioEnvironment::IndoorMall: return 4.5f;
	case EGZAudioEnvironment::Tunnel:     return 6.2f;
	case EGZAudioEnvironment::River:      return 3.0f;
	default: return 1.2f;
	}
}

FGZAudioEnvironmentSettings UGZAudioManager::GetEnvironmentSettings(EGZAudioEnvironment Environment)
{
	FGZAudioEnvironmentSettings Settings;
	Settings.Environment = Environment;
	Settings.ReverbTime = GetEnvironmentReverbTime(Environment);

	switch (Environment)
	{
	case EGZAudioEnvironment::Street:
		Settings.ReverbDamping = 0.3f;
		Settings.OcclusionFactor = 0.1f;
		Settings.DiffractionFactor = 0.2f;
		break;
	case EGZAudioEnvironment::QilouAlley:
		Settings.ReverbDamping = 0.4f;
		Settings.OcclusionFactor = 0.3f;
		Settings.DiffractionFactor = 0.4f;
		break;
	case EGZAudioEnvironment::IndoorMall:
		Settings.ReverbDamping = 0.5f;
		Settings.OcclusionFactor = 0.5f;
		Settings.DiffractionFactor = 0.3f;
		break;
	case EGZAudioEnvironment::Tunnel:
		Settings.ReverbDamping = 0.6f;
		Settings.OcclusionFactor = 0.7f;
		Settings.DiffractionFactor = 0.5f;
		break;
	case EGZAudioEnvironment::River:
		Settings.ReverbDamping = 0.2f;
		Settings.OcclusionFactor = 0.05f;
		Settings.DiffractionFactor = 0.1f;
		break;
	}

	return Settings;
}

UGZAudioManager::UGZAudioManager()
{
}

void UGZAudioManager::Initialize()
{
	ActiveSources.Empty();
	ActiveSources.Reserve(256);

	EnvironmentSettings.Empty();
	for (int32 i = 0; i < 5; ++i)
	{
		EnvironmentSettings.Add(GetEnvironmentSettings(static_cast<EGZAudioEnvironment>(i)));
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Audio: SoLoud v%.2f, CoreAudio %.1f, %.0fkHz, %.0fms latency, 5 environments: Street=%.1fs, Qilou=%.1fs, Indoor=%.1fs, Tunnel=%.1fs, River=%.1fs, reverb transition=%.1fs"),
		SoLoudVersion, CoreAudioVersion, SampleRate / 1000.0f, AudioLatency * 1000.0f,
		GetEnvironmentReverbTime(EGZAudioEnvironment::Street),
		GetEnvironmentReverbTime(EGZAudioEnvironment::QilouAlley),
		GetEnvironmentReverbTime(EGZAudioEnvironment::IndoorMall),
		GetEnvironmentReverbTime(EGZAudioEnvironment::Tunnel),
		GetEnvironmentReverbTime(EGZAudioEnvironment::River),
		ReverbTransitionTime);
}

void UGZAudioManager::Tick(float DeltaTime, const FVector& InListenerPosition, const FVector& InListenerVelocity)
{
	ListenerPosition = InListenerPosition;
	ListenerVelocity = InListenerVelocity;

	UpdateSourceAttenuation(DeltaTime, ListenerPosition);
	UpdateReverbTransition(DeltaTime);
	UpdateDopplerEffect(DeltaTime, ListenerPosition, ListenerVelocity);

	for (FGZAudioSource& Source : ActiveSources)
	{
		if (Source.bIsPlaying)
		{
			ApplyOcclusion(ListenerPosition, Source);
			ApplyHRTFFilter(Source, ListenerPosition);
			UpdateSpatialParameters(Source, ListenerPosition);
		}
	}
}

int32 UGZAudioManager::PlaySound(int32 SoundID, const FVector& Position, float Volume, bool bLoop)
{
	FGZAudioSource Source;
	Source.SourceID = NextSourceID++;
	Source.Position = Position;
	Source.Velocity = FVector::ZeroVector;
	Source.Volume = Volume * MasterVolume;
	Source.Pitch = 1.0f;
	Source.bIs3D = true;
	Source.bIsLooping = bLoop;
	Source.bIsPlaying = true;
	Source.Environment = CurrentEnvironment;
	Source.CurrentReverbTime = GetEnvironmentReverbTime(CurrentEnvironment);
	Source.TargetReverbTime = Source.CurrentReverbTime;
	Source.ReverbTransitionProgress = 1.0f;

	ActiveSources.Add(Source);

	return Source.SourceID;
}

void UGZAudioManager::StopSound(int32 SourceID)
{
	for (int32 i = ActiveSources.Num() - 1; i >= 0; --i)
	{
		if (ActiveSources[i].SourceID == SourceID)
		{
			ActiveSources[i].bIsPlaying = false;
			ActiveSources.RemoveAtSwap(i);
			return;
		}
	}
}

void UGZAudioManager::SetSourcePosition(int32 SourceID, const FVector& NewPosition)
{
	for (FGZAudioSource& Source : ActiveSources)
	{
		if (Source.SourceID == SourceID)
		{
			Source.Position = NewPosition;
			return;
		}
	}
}

void UGZAudioManager::SetSourceVelocity(int32 SourceID, const FVector& NewVelocity)
{
	for (FGZAudioSource& Source : ActiveSources)
	{
		if (Source.SourceID == SourceID)
		{
			Source.Velocity = NewVelocity;
			return;
		}
	}
}

void UGZAudioManager::SetEnvironment(EGZAudioEnvironment NewEnvironment)
{
	if (TargetEnvironment == NewEnvironment) return;

	TargetEnvironment = NewEnvironment;
	ReverbTransitionTimer = 0.0f;

	for (FGZAudioSource& Source : ActiveSources)
	{
		Source.TargetReverbTime = GetEnvironmentReverbTime(NewEnvironment);
		Source.ReverbTransitionProgress = 0.0f;
	}
}

void UGZAudioManager::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void UGZAudioManager::SetListenerPosition(const FVector& Position, const FVector& Velocity)
{
	ListenerPosition = Position;
	ListenerVelocity = Velocity;
}

void UGZAudioManager::UpdateSourceAttenuation(float DeltaTime, const FVector& InListenerPosition)
{
	for (FGZAudioSource& Source : ActiveSources)
	{
		if (!Source.bIsPlaying || !Source.bIs3D) continue;

		float Distance = FVector::Dist(Source.Position, InListenerPosition);
		float Attenuation = CalculateAttenuation(Distance);
		Source.Volume = Attenuation * MasterVolume;
	}
}

void UGZAudioManager::UpdateReverbTransition(float DeltaTime)
{
	if (CurrentEnvironment == TargetEnvironment) return;

	ReverbTransitionTimer += DeltaTime;
	float Progress = FMath::Min(ReverbTransitionTimer / ReverbTransitionTime, 1.0f);

	for (FGZAudioSource& Source : ActiveSources)
	{
		Source.ReverbTransitionProgress = Progress;
		Source.CurrentReverbTime = FMath::Lerp(
			GetEnvironmentReverbTime(CurrentEnvironment),
			Source.TargetReverbTime,
			Progress
		);
	}

	if (Progress >= 1.0f)
	{
		CurrentEnvironment = TargetEnvironment;
	}
}

void UGZAudioManager::UpdateDopplerEffect(float DeltaTime, const FVector& InListenerPosition, const FVector& InListenerVelocity)
{
	for (FGZAudioSource& Source : ActiveSources)
	{
		if (!Source.bIsPlaying || !Source.bIs3D) continue;

		FVector SourceToListener = InListenerPosition - Source.Position;
		float Distance = SourceToListener.Size();
		if (Distance < 1.0f) continue;

		FVector RelativeVelocity = Source.Velocity - InListenerVelocity;
		FVector Direction = SourceToListener / Distance;
		float RelativeSpeed = FVector::DotProduct(RelativeVelocity, Direction);

		float DopplerFactor = 1.0f / (1.0f + RelativeSpeed / SpeedOfSound);
		Source.Pitch = FMath::Clamp(DopplerFactor, 0.5f, 2.0f);
	}
}

void UGZAudioManager::ApplyOcclusion(const FVector& InListenerPosition, FGZAudioSource& Source)
{
	float Distance = FVector::Dist(Source.Position, InListenerPosition);
	FGZAudioEnvironmentSettings EnvSettings = GetEnvironmentSettings(Source.Environment);

	float Occlusion = EnvSettings.OcclusionFactor * (1.0f - FMath::Exp(-Distance / 1000.0f));
	Source.Volume *= (1.0f - Occlusion);
}

float UGZAudioManager::CalculateAttenuation(float Distance) const
{
	if (Distance <= NearDistance)
	{
		return NearAttenuation;
	}
	else if (Distance <= MediumDistance)
	{
		float T = (Distance - NearDistance) / (MediumDistance - NearDistance);
		return FMath::Lerp(NearAttenuation, MediumAttenuation, T);
	}
	else
	{
		float T = FMath::Clamp((Distance - MediumDistance) / 10000.0f, 0.0f, 1.0f);
		return FMath::Lerp(MediumAttenuation, FarAttenuation, T);
	}
}

void UGZAudioManager::ApplyHRTFFilter(FGZAudioSource& Source, const FVector& InListenerPosition)
{
	FVector Delta = Source.Position - InListenerPosition;
	float Distance = Delta.Size();

	if (Distance < 1.0f) return;

	FVector Direction = Delta / Distance;
	FVector Forward(1.0f, 0.0f, 0.0f);
	FVector Right(0.0f, 1.0f, 0.0f);

	float Azimuth = FMath::Atan2(FVector::DotProduct(Direction, Right), FVector::DotProduct(Direction, Forward));
	float Elevation = FMath::Asin(Direction.Z);

	float ITD = (FMath::Sin(Azimuth) * 0.00065f);
	float ILD = FMath::Clamp(1.0f - FMath::Abs(Azimuth) / PI, 0.2f, 1.0f);
}

void UGZAudioManager::UpdateSpatialParameters(FGZAudioSource& Source, const FVector& InListenerPosition)
{
	FVector Delta = Source.Position - InListenerPosition;
	float Distance = Delta.Size();

	if (Distance < 1.0f) return;
}