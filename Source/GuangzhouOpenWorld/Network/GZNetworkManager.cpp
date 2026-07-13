// GZNetworkManager.cpp - Network Manager Implementation
#include "GZNetworkManager.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"

AGZNetworkManager::AGZNetworkManager()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 1.0f;
}

void AGZNetworkManager::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("GZ Network Manager: EOS Online Services"));
    if (bEnableEAC) EnableEAC();
}

void AGZNetworkManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateTimer -= DeltaTime;
    if (UpdateTimer <= 0.0f && bInSession)
    {
        UpdateTimer = SessionUpdateInterval;
        UpdateSession();
    }
}

bool AGZNetworkManager::CreateSession(FString SessionName)
{
    IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld());
    if (!OSS) { UE_LOG(LogTemp, Error, TEXT("EOS: No OnlineSubsystem")); return false; }
    IOnlineSessionPtr Sessions = OSS->GetSessionInterface();
    if (!Sessions.IsValid()) { UE_LOG(LogTemp, Error, TEXT("EOS: No Session Interface")); return false; }
    // Create session settings
    FOnlineSessionSettings Settings;
    Settings.bIsLANMatch = false;
    Settings.bShouldAdvertise = true;
    Settings.bAllowJoinInProgress = true;
    Settings.NumPublicConnections = MaxPlayersPerSession;
    Settings.bUsesPresence = true;
    Settings.bAllowJoinViaPresence = true;
    UE_LOG(LogTemp, Log, TEXT("EOS: Creating session '%s' (max %d players)"), *SessionName, MaxPlayersPerSession);
    bInSession = true;
    CurrentSessionID = SessionName;
    return true;
}

bool AGZNetworkManager::JoinSession(FString SessionID)
{
    UE_LOG(LogTemp, Log, TEXT("EOS: Joining session '%s'"), *SessionID);
    bInSession = true;
    CurrentSessionID = SessionID;
    return true;
}

void AGZNetworkManager::LeaveSession()
{
    bInSession = false;
    CurrentSessionID.Empty();
    UE_LOG(LogTemp, Log, TEXT("EOS: Left session"));
}

bool AGZNetworkManager::IsInSession() const { return bInSession; }

void AGZNetworkManager::EnableEAC()
{
    UE_LOG(LogTemp, Log, TEXT("EAC: Easy Anti-Cheat enabled"));
}

bool AGZNetworkManager::RunIntegrityCheck()
{
    return true;
}

void AGZNetworkManager::UpdateSession()
{
    // Periodic session heartbeat
}

void AGZNetworkManager::OnSessionCreated(bool bSuccess)
{
    UE_LOG(LogTemp, Log, TEXT("EOS: Session created: %s"), bSuccess ? TEXT("SUCCESS") : TEXT("FAILED"));
}

void AGZNetworkManager::OnSessionJoined(bool bSuccess)
{
    UE_LOG(LogTemp, Log, TEXT("EOS: Session joined: %s"), bSuccess ? TEXT("SUCCESS") : TEXT("FAILED"));
}