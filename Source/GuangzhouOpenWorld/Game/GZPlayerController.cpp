#include "GZPlayerController.h"
#include "GZGameMode.h"
#include "GZCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AGZPlayerController::AGZPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
    bShowMouseCursor = false;
    bEnableClickEvents = false;
    bEnableMouseOverEvents = false;
}

void AGZPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetInputMode(FInputModeGameOnly());
    UE_LOG(LogTemp, Log, TEXT("GZPlayerController BeginPlay"));
}

void AGZPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindAxis("MoveForward", this, &AGZPlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AGZPlayerController::MoveRight);
    InputComponent->BindAxis("LookUp", this, &AGZPlayerController::LookUp);
    InputComponent->BindAxis("LookRight", this, &AGZPlayerController::LookRight);
    InputComponent->BindAction("Jump", IE_Pressed, this, &AGZPlayerController::ExitVehicle);
    InputComponent->BindAction("Shoot", IE_Pressed, this, &AGZPlayerController::Shoot);
    InputComponent->BindAction("Interact", IE_Pressed, this, &AGZPlayerController::EnterVehicle);
    InputComponent->BindAction("Reload", IE_Pressed, this, &AGZPlayerController::ReloadWeapon);
    InputComponent->BindAction("Weapon1", IE_Pressed, this, &AGZPlayerController::SwitchWeapon, 0);
    InputComponent->BindAction("Weapon2", IE_Pressed, this, &AGZPlayerController::SwitchWeapon, 1);
    InputComponent->BindAction("Weapon3", IE_Pressed, this, &AGZPlayerController::SwitchWeapon, 2);
    InputComponent->BindAction("Weapon4", IE_Pressed, this, &AGZPlayerController::SwitchWeapon, 3);
    InputComponent->BindAction("Weapon5", IE_Pressed, this, &AGZPlayerController::SwitchWeapon, 4);
}

void AGZPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (WantedLevel > 0)
    {
        WantedTimer -= DeltaTime;
        if (WantedTimer <= 0.0f) { WantedLevel = FMath::Max(0, WantedLevel - 1); WantedTimer = 20.0f; }
    }
    // Auto quality
    if (AGZGameMode* GM = Cast<AGZGameMode>(GetWorld()->GetAuthGameMode()))
    {
        GM->UpdateAutoQuality(1.0f / FMath::Max(DeltaTime, 0.001f));
    }
}

void AGZPlayerController::MoveForward(float Value) { if (APawn* P = GetPawn()) P->AddMovementInput(P->GetActorForwardVector(), Value); }
void AGZPlayerController::MoveRight(float Value) { if (APawn* P = GetPawn()) P->AddMovementInput(P->GetActorRightVector(), Value); }
void AGZPlayerController::LookUp(float Value) { AddPitchInput(Value); }
void AGZPlayerController::LookRight(float Value) { AddYawInput(Value); }

void AGZPlayerController::TakeDamage(float Damage)
{
    if (Armor > 0) { float AD = FMath::Min(Armor, Damage * 0.7f); Armor -= AD; Health -= Damage - AD; }
    else Health -= Damage;
    Health = FMath::Max(0.0f, Health);
}

void AGZPlayerController::AddWantedLevel(int32 Amount)
{
    WantedLevel = FMath::Min(6, WantedLevel + Amount);
    WantedTimer = 30.0f + WantedLevel * 5.0f;
}

void AGZPlayerController::SwitchWeapon(int32 Index)
{
    CurrentWeaponIndex = FMath::Clamp(Index, 0, WeaponNames.Num() - 1);
}

void AGZPlayerController::ReloadWeapon()
{
    if (CurrentWeaponIndex > 0 && CurrentWeaponIndex < AmmoCounts.Num())
        AmmoCounts[CurrentWeaponIndex] = MaxAmmo[CurrentWeaponIndex];
}

void AGZPlayerController::Shoot()
{
    if (CurrentWeaponIndex == 0) return; // Fist
    if (AmmoCounts[CurrentWeaponIndex] <= 0) return;
    float Now = GetWorld()->GetTimeSeconds();
    float FireRate = (CurrentWeaponIndex == 2) ? 0.12f : (CurrentWeaponIndex == 3) ? 0.8f : 0.4f;
    if (Now - LastShotTime < FireRate) return;
    LastShotTime = Now;
    AmmoCounts[CurrentWeaponIndex]--;
    AddWantedLevel(1);
    UE_LOG(LogTemp, Log, TEXT("Shot fired! Weapon: %s, Ammo: %d"), *WeaponNames[CurrentWeaponIndex], AmmoCounts[CurrentWeaponIndex]);
}

void AGZPlayerController::EnterVehicle()
{
    bInVehicle = true;
    UE_LOG(LogTemp, Log, TEXT("Entered vehicle"));
}

void AGZPlayerController::ExitVehicle()
{
    bInVehicle = false;
    UE_LOG(LogTemp, Log, TEXT("Exited vehicle"));
}