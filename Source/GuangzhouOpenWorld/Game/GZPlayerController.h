// GZPlayerController.h - GTA-广州 Player Controller
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GZPlayerController.generated.h"

UCLASS()
class GUANGZHOUOPENWORLD_API AGZPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    AGZPlayerController();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupInputComponent() override;

    // Player state
    UPROPERTY(BlueprintReadOnly) float Health = 100.0f;
    UPROPERTY(BlueprintReadOnly) float Armor = 0.0f;
    UPROPERTY(BlueprintReadOnly) int32 Money = 5000;
    UPROPERTY(BlueprintReadOnly) int32 WantedLevel = 0;
    UPROPERTY(BlueprintReadOnly) bool bInVehicle = false;
    UPROPERTY(BlueprintReadOnly) int32 CurrentWeaponIndex = 1;

    // Weapons
    UPROPERTY(BlueprintReadOnly) TArray<FString> WeaponNames = {TEXT("Fist"), TEXT("Pistol"), TEXT("Rifle"), TEXT("Shotgun"), TEXT("Sniper")};
    UPROPERTY(BlueprintReadOnly) TArray<int32> AmmoCounts = {0, 60, 90, 24, 15};
    UPROPERTY(BlueprintReadOnly) TArray<int32> MaxAmmo = {0, 60, 90, 24, 15};

    UFUNCTION(BlueprintCallable) void TakeDamage(float Damage);
    UFUNCTION(BlueprintCallable) void AddWantedLevel(int32 Amount);
    UFUNCTION(BlueprintCallable) void SwitchWeapon(int32 Index);
    UFUNCTION(BlueprintCallable) void ReloadWeapon();
    UFUNCTION(BlueprintCallable) void Shoot();
    UFUNCTION(BlueprintCallable) void EnterVehicle();
    UFUNCTION(BlueprintCallable) void ExitVehicle();

private:
    float LastShotTime = 0.0f;
    float WantedTimer = 0.0f;
    void MoveForward(float Value);
    void MoveRight(float Value);
    void LookUp(float Value);
    void LookRight(float Value);
};