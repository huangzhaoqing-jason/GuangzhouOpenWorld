// GZAudioManager.h - 3D Spatial Audio for Guangzhou Open World
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GZAudioManager.generated.h"

UENUM(BlueprintType)
enum class EAudioEnvironment : uint8
{
    Street = 0 UMETA(DisplayName="Street"),
    Indoor = 1 UMETA(DisplayName="Indoor"),
    Tunnel = 2 UMETA(DisplayName="Tunnel"),
    River = 3 UMETA(DisplayName="River"),
};

UCLASS()
class GUANGZHOUOPENWORLD_API AGZAudioManager : public AActor
{
    GENERATED_BODY()
public:
    AGZAudioManager();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere) float MasterVolume = 1.0f;
    UPROPERTY(EditAnywhere) float SFXVolume = 1.0f;
    UPROPERTY(EditAnywhere) float MusicVolume = 0.8f;
    UPROPERTY(EditAnywhere) float AmbientVolume = 0.6f;
    UPROPERTY(EditAnywhere) float MaxAudioDistance = 10000.0f;

    UFUNCTION(BlueprintCallable) void SetEnvironment(EAudioEnvironment Env);
    UFUNCTION(BlueprintCallable) void PlaySFX(USoundBase* Sound, FVector Location, float Volume = 1.0f);
    UFUNCTION(BlueprintCallable) void PlayAmbient(FVector Location, EAudioEnvironment Type);
    UFUNCTION(BlueprintCallable) void UpdateAudioListener(FVector ListenerPos, FRotator ListenerRot);

    // Environment-specific reverb parameters
    UPROPERTY(EditAnywhere) float StreetReverb = 0.3f;
    UPROPERTY(EditAnywhere) float IndoorReverb = 0.6f;
    UPROPERTY(EditAnywhere) float TunnelReverb = 0.8f;
    UPROPERTY(EditAnywhere) float RiverReverb = 0.4f;

    // Environmental audio occlusion
    UPROPERTY(EditAnywhere) bool bEnableOcclusion = true;
    UPROPERTY(EditAnywhere) float OcclusionRayDistance = 5000.0f;

private:
    EAudioEnvironment CurrentEnv = EAudioEnvironment::Street;
    EAudioEnvironment PreviousEnv = EAudioEnvironment::Street;
    float EnvBlendAlpha = 0.0f;
    float BlendTimer = 0.0f;
    void UpdateEnvironmentBlend(float DeltaTime);
    float GetEnvironmentReverb(EAudioEnvironment Env) const;
};