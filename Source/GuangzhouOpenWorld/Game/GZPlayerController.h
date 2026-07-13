#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GZPlayerController.generated.h"

UENUM(BlueprintType)
enum class EGZWeaponType : uint8
{
	Fist		UMETA(DisplayName = "Fist"),
	Pistol		UMETA(DisplayName = "Pistol"),
	Rifle		UMETA(DisplayName = "Rifle"),
	Shotgun		UMETA(DisplayName = "Shotgun"),
	Sniper		UMETA(DisplayName = "Sniper"),
};

USTRUCT(BlueprintType)
struct FGZWeaponState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZWeaponType Type = EGZWeaponType::Fist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ReserveAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsReloading = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadProgress = 0.0f;
};

UCLASS()
class GUANGZHOUOPENWORLD_API AGZPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGZPlayerController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float DamageAmount, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetArmor() const { return Armor; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxArmor() const { return MaxArmor; }

	UFUNCTION(BlueprintCallable, Category = "Money")
	void AddMoney(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Money")
	bool SpendMoney(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Money")
	int32 GetMoney() const { return Money; }

	UFUNCTION(BlueprintCallable, Category = "Wanted")
	void SetWantedLevel(int32 NewLevel);

	UFUNCTION(BlueprintPure, Category = "Wanted")
	int32 GetWantedLevel() const { return WantedLevel; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SwitchWeapon(EGZWeaponType NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReloadWeapon();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EGZWeaponType GetCurrentWeapon() const { return CurrentWeapon.Type; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurrentAmmo() const { return CurrentWeapon.CurrentAmmo; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetReserveAmmo() const { return CurrentWeapon.ReserveAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void EnterVehicle(class AActor* Vehicle);

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void ExitVehicle();

	UFUNCTION(BlueprintPure, Category = "Vehicle")
	bool IsInVehicle() const { return bInVehicle; }

	UFUNCTION(BlueprintCallable, Category = "Lighting")
	void DetectAndUpdateLightingZone();

protected:
	void UpdateWantedDecay(float DeltaSeconds);
	void UpdateReload(float DeltaSeconds);
	void InitializeWeapons();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void StartSprint();
	void StopSprint();
	void StartCrouch();
	void StopCrouch();
	void FireAction();
	void ReloadAction();
	void SwitchWeaponAction(int32 WeaponIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Armor = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxArmor = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Money")
	int32 Money = 5000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wanted")
	int32 WantedLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wanted")
	float WantedDecayTime = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wanted")
	float WantedDecayTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wanted")
	float WantedDecayRate = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FGZWeaponState CurrentWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<FGZWeaponState> WeaponInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	bool bInVehicle = false;

	UPROPERTY()
	class AActor* CurrentVehicle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
	float LightingZoneCheckInterval = 1.0f;

	UPROPERTY()
	float LightingZoneCheckTimer = 0.0f;

	UPROPERTY()
	class AGZGameMode* CachedGameMode = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* InputMappingContext = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MoveAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* LookAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* SprintAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* CrouchAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* FireAction_Input = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* ReloadAction_Input = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* SwitchWeapon1Action = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* SwitchWeapon2Action = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* SwitchWeapon3Action = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* SwitchWeapon4Action = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* SwitchWeapon5Action = nullptr;
};