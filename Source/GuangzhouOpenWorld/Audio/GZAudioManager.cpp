// GZAudioManager.cpp - Audio Manager Implementation
#include "GZAudioManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "AudioDevice.h"

AGZAudioManager::AGZAudioManager()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 1.0f / 30.0f;
}

void AGZAudioManager::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("GZ Audio Manager: Core Audio | 3D Spatial | 4 Environments"));
}

void AGZAudioManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateEnvironmentBlend(DeltaTime);
}

void AGZAudioManager::SetEnvironment(EAudioEnvironment Env)
{
    if (Env != CurrentEnv)
    {
        PreviousEnv = CurrentEnv;
        CurrentEnv = Env;
        BlendTimer = 0.0f;
        UE_LOG(LogTemp, Verbose, TEXT("Audio Environment: %d -> %d"), (int32)PreviousEnv, (int32)CurrentEnv);
    }
}

void AGZAudioManager::UpdateEnvironmentBlend(float DeltaTime)
{
    BlendTimer += DeltaTime;
    EnvBlendAlpha = FMath::Clamp(BlendTimer / 0.5f, 0.0f, 1.0f);
    float PrevReverb = GetEnvironmentReverb(PreviousEnv);
    float CurrReverb = GetEnvironmentReverb(CurrentEnv);
    float BlendedReverb = FMath::Lerp(PrevReverb, CurrReverb, EnvBlendAlpha);
    // Apply blended reverb to audio engine
    if (FAudioDevice* AudioDevice = GetWorld()->GetAudioDeviceRaw())
    {
        // AudioDevice->SetReverbAmount(BlendedReverb);
    }
}

float AGZAudioManager::GetEnvironmentReverb(EAudioEnvironment Env) const
{
    switch (Env)
    {
        case EAudioEnvironment::Street: return StreetReverb;
        case EAudioEnvironment::Indoor: return IndoorReverb;
        case EAudioEnvironment::Tunnel: return TunnelReverb;
        case EAudioEnvironment::River: return RiverReverb;
        default: return 0.3f;
    }
}

void AGZAudioManager::PlaySFX(USoundBase* Sound, FVector Location, float Volume)
{
    if (Sound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, Volume * SFXVolume * MasterVolume);
}

void AGZAudioManager::PlayAmbient(FVector Location, EAudioEnvironment Type)
{
    // Ambient audio based on environment type
    // In full implementation, this would load and play appropriate ambient loops
}

void AGZAudioManager::UpdateAudioListener(FVector ListenerPos, FRotator ListenerRot)
{
    // Update audio listener position for 3D spatialization
    if (FAudioDevice* AudioDevice = GetWorld()->GetAudioDeviceRaw())
    {
        // AudioDevice->SetListener(0, ListenerPos, ListenerRot.Vector());
    }
}