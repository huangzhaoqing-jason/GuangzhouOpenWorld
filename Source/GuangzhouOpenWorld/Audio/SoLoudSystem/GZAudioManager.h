#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZAudioManager.generated.h"

UENUM(BlueprintType)
enum class EGZAudioEnvironment : uint8
{
	Street			UMETA(DisplayName = "Street"),
	QilouAlley		UMETA(DisplayName = "Qilou Alley"),
	IndoorMall		UMETA(DisplayName = "Indoor Mall"),
	Tunnel			UMETA(DisplayName = "Tunnel"),
	River			UMETA(DisplayName = "River"),
};

USTRUCT(BlueprintType)
struct FGZAudioSource
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SourceID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Volume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Pitch = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIs3D = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLooping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPlaying = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZAudioEnvironment Environment = EGZAudioEnvironment::Street;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentReverbTime = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetReverbTime = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReverbTransitionProgress = 1.0f;
};

USTRUCT(BlueprintType)
struct FGZAudioEnvironmentSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZAudioEnvironment Environment = EGZAudioEnvironment::Street;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReverbTime = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReverbDamping = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OcclusionFactor = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DiffractionFactor = 0.0f;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZAudioManager : public UObject
{
	GENERATED_BODY()

public:
	UGZAudioManager();

	void Initialize();
	void Tick(float DeltaTime, const FVector& ListenerPosition, const FVector& ListenerVelocity);
	int32 PlaySound(int32 SoundID, const FVector& Position, float Volume = 1.0f, bool bLoop = false);
	void StopSound(int32 SourceID);
	void SetSourcePosition(int32 SourceID, const FVector& NewPosition);
	void SetSourceVelocity(int32 SourceID, const FVector& NewVelocity);
	void SetEnvironment(EGZAudioEnvironment NewEnvironment);
	void SetMasterVolume(float Volume);
	void SetListenerPosition(const FVector& Position, const FVector& Velocity);

	UFUNCTION(BlueprintPure)
	EGZAudioEnvironment GetCurrentEnvironment() const { return CurrentEnvironment; }

	UFUNCTION(BlueprintPure)
	int32 GetActiveSourceCount() const { return ActiveSources.Num(); }

	static float GetEnvironmentReverbTime(EGZAudioEnvironment Environment);
	static FGZAudioEnvironmentSettings GetEnvironmentSettings(EGZAudioEnvironment Environment);

private:
	void UpdateSourceAttenuation(float DeltaTime, const FVector& ListenerPosition);
	void UpdateReverbTransition(float DeltaTime);
	void UpdateDopplerEffect(float DeltaTime, const FVector& ListenerPosition, const FVector& ListenerVelocity);
	void ApplyOcclusion(const FVector& ListenerPosition, FGZAudioSource& Source);
	float CalculateAttenuation(float Distance) const;
	void ApplyHRTFFilter(FGZAudioSource& Source, const FVector& ListenerPosition);
	void UpdateSpatialParameters(FGZAudioSource& Source, const FVector& ListenerPosition);

	UPROPERTY()
	TArray<FGZAudioSource> ActiveSources;

	UPROPERTY()
	TArray<FGZAudioEnvironmentSettings> EnvironmentSettings;

	EGZAudioEnvironment CurrentEnvironment = EGZAudioEnvironment::Street;
	EGZAudioEnvironment TargetEnvironment = EGZAudioEnvironment::Street;

	float MasterVolume = 1.0f;
	float ReverbTransitionTimer = 0.0f;
	int32 NextSourceID = 1;

	FVector ListenerPosition = FVector::ZeroVector;
	FVector ListenerVelocity = FVector::ZeroVector;

	static constexpr float SoLoudVersion = 2026.0f;
	static constexpr float CoreAudioVersion = 14.3f;
	static constexpr float AudioLatency = 0.016f;
	static constexpr float SampleRate = 48000.0f;
	static constexpr float ReverbTransitionTime = 0.5f;
	static constexpr float NearDistance = 500.0f;
	static constexpr float MediumDistance = 2000.0f;
	static constexpr float NearAttenuation = 1.0f;
	static constexpr float MediumAttenuation = 0.4f;
	static constexpr float FarAttenuation = 0.1f;
	static constexpr float SpeedOfSound = 343.0f;
};