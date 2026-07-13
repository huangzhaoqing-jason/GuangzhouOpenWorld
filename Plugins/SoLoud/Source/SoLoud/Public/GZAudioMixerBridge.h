#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZAudioMixerBridge.generated.h"

UENUM(BlueprintType)
enum class EGZAudioChannelGroup : uint8
{
	Master     UMETA(DisplayName="Master"),
	Ambient    UMETA(DisplayName="Ambient"),
	Weather    UMETA(DisplayName="Weather"),
	Vehicle    UMETA(DisplayName="Vehicle"),
	Voice      UMETA(DisplayName="Voice"),
};

USTRUCT(BlueprintType)
struct FZSpatialAudioParams
{
	GENERATED_BODY()

	UPROPERTY() FVector Location;
	UPROPERTY() FVector Velocity;
	UPROPERTY() float MaxDistance = 10000.0f;
	UPROPERTY() float Volume = 1.0f;
	UPROPERTY() float Pitch = 1.0f;
	UPROPERTY() bool bLooping = false;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOLOUD_API UGZAudioMixerBridge : public UObject
{
	GENERATED_BODY()

public:
	UGZAudioMixerBridge();

	UFUNCTION(BlueprintCallable, Category="SoLoudAudio")
	void Initialize();

	UFUNCTION(BlueprintCallable, Category="SoLoudAudio")
	void Shutdown();

	UFUNCTION(BlueprintCallable, Category="SoLoudAudio")
	void SetGroupVolume(EGZAudioChannelGroup Group, float Volume);

	UFUNCTION(BlueprintCallable, Category="SoLoudAudio")
	float GetGroupVolume(EGZAudioChannelGroup Group) const;

	UFUNCTION(BlueprintCallable, Category="SoLoudAudio")
	void PlayEvent(const FString& EventPath, const FZSpatialAudioParams& Params);

	UFUNCTION(BlueprintCallable, Category="SoLoudAudio")
	void StopAllSounds();

	UFUNCTION(BlueprintCallable, Category="SoLoudAudio")
	void PauseAllSounds();

	UFUNCTION(BlueprintCallable, Category="SoLoudAudio")
	void ResumeAllSounds();

	UFUNCTION(BlueprintCallable, Category="SoLoudAudio")
	void SetGlobalPause(bool bPaused);

	UFUNCTION(BlueprintCallable, Category="SoLoudAudio")
	void SetListenerTransform(const FVector& Location, const FVector& Forward);

	UFUNCTION(BlueprintCallable, Category="SoLoudAudio")
	float ComputeAttenuation(float Distance, float MaxDistance) const;

private:
	TMap<EGZAudioChannelGroup, float> GroupVolumes;
	FVector ListenerLocation;
	FVector ListenerForward;
	bool bGlobalPaused = false;
	int32 MaxSimultaneousSounds = 64;

	float LogarithmicToLinear(float Volume01) const;
};
