// GZNetworkManager.h - EOS Online Services for Guangzhou Open World
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GZNetworkManager.generated.h"

UCLASS()
class GUANGZHOUOPENWORLD_API AGZNetworkManager : public AActor
{
    GENERATED_BODY()
public:
    AGZNetworkManager();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere) FString EOSProductID = TEXT("gzcity-guangzhou");
    UPROPERTY(EditAnywhere) FString EOSClientID = TEXT("xyz");
    UPROPERTY(EditAnywhere) FString EOSClientSecret = TEXT("");
    UPROPERTY(EditAnywhere) bool bEnableEAC = true;
    UPROPERTY(EditAnywhere) int32 MaxPlayersPerSession = 64;
    UPROPERTY(EditAnywhere) float SessionUpdateInterval = 1.0f;

    // Session management
    UFUNCTION(BlueprintCallable) bool CreateSession(FString SessionName);
    UFUNCTION(BlueprintCallable) bool JoinSession(FString SessionID);
    UFUNCTION(BlueprintCallable) void LeaveSession();
    UFUNCTION(BlueprintCallable) bool IsInSession() const;

    // Anti-cheat
    UFUNCTION(BlueprintCallable) void EnableEAC();
    UFUNCTION(BlueprintCallable) bool RunIntegrityCheck();

    // Server authoritative
    UFUNCTION(BlueprintCallable) bool IsServerAuthoritative() const { return true; }

private:
    bool bInSession = false;
    FString CurrentSessionID;
    float UpdateTimer = 0.0f;
    void UpdateSession();
    void OnSessionCreated(bool bSuccess);
    void OnSessionJoined(bool bSuccess);
};