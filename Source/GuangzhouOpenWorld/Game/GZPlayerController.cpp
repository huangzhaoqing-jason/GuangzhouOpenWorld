#include "Game/GZPlayerController.h"
#include "Game/GZGameMode.h"
#include "GuangzhouOpenWorld.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"

AGZPlayerController::AGZPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f / 60.0f;
}

void AGZPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CachedGameMode = Cast<AGZGameMode>(GetWorld()->GetAuthGameMode());

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	InitializeWeapons();
}

void AGZPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateWantedDecay(DeltaSeconds);
	UpdateReload(DeltaSeconds);

	LightingZoneCheckTimer += DeltaSeconds;
	if (LightingZoneCheckTimer >= LightingZoneCheckInterval)
	{
		LightingZoneCheckTimer = 0.0f;
		DetectAndUpdateLightingZone();
	}
}

void AGZPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGZPlayerController::MoveForward);
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGZPlayerController::MoveRight);
		}
		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGZPlayerController::Turn);
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGZPlayerController::LookUp);
		}
		if (SprintAction)
		{
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &AGZPlayerController::StartSprint);
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AGZPlayerController::StopSprint);
		}
		if (CrouchAction)
		{
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &AGZPlayerController::StartCrouch);
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AGZPlayerController::StopCrouch);
		}
		if (FireAction_Input)
		{
			EnhancedInput->BindAction(FireAction_Input, ETriggerEvent::Started, this, &AGZPlayerController::FireAction);
		}
		if (ReloadAction_Input)
		{
			EnhancedInput->BindAction(ReloadAction_Input, ETriggerEvent::Started, this, &AGZPlayerController::ReloadAction);
		}
		if (SwitchWeapon1Action)
		{
			EnhancedInput->BindAction(SwitchWeapon1Action, ETriggerEvent::Started, this, &AGZPlayerController::SwitchWeaponAction, 0);
		}
		if (SwitchWeapon2Action)
		{
			EnhancedInput->BindAction(SwitchWeapon2Action, ETriggerEvent::Started, this, &AGZPlayerController::SwitchWeaponAction, 1);
		}
		if (SwitchWeapon3Action)
		{
			EnhancedInput->BindAction(SwitchWeapon3Action, ETriggerEvent::Started, this, &AGZPlayerController::SwitchWeaponAction, 2);
		}
		if (SwitchWeapon4Action)
		{
			EnhancedInput->BindAction(SwitchWeapon4Action, ETriggerEvent::Started, this, &AGZPlayerController::SwitchWeaponAction, 3);
		}
		if (SwitchWeapon5Action)
		{
			EnhancedInput->BindAction(SwitchWeapon5Action, ETriggerEvent::Started, this, &AGZPlayerController::SwitchWeaponAction, 4);
		}
	}
}

void AGZPlayerController::InitializeWeapons()
{
	WeaponInventory.Empty();

	FGZWeaponState Fist;
	Fist.Type = EGZWeaponType::Fist;
	Fist.Damage = 10.0f;
	Fist.Range = 150.0f;
	Fist.FireRate = 0.3f;
	WeaponInventory.Add(Fist);

	FGZWeaponState Pistol;
	Pistol.Type = EGZWeaponType::Pistol;
	Pistol.CurrentAmmo = 12;
	Pistol.MaxAmmo = 12;
	Pistol.ReserveAmmo = 60;
	Pistol.FireRate = 0.15f;
	Pistol.ReloadTime = 1.5f;
	Pistol.Damage = 25.0f;
	Pistol.Range = 5000.0f;
	WeaponInventory.Add(Pistol);

	FGZWeaponState Rifle;
	Rifle.Type = EGZWeaponType::Rifle;
	Rifle.CurrentAmmo = 30;
	Rifle.MaxAmmo = 30;
	Rifle.ReserveAmmo = 180;
	Rifle.FireRate = 0.08f;
	Rifle.ReloadTime = 2.0f;
	Rifle.Damage = 30.0f;
	Rifle.Range = 15000.0f;
	WeaponInventory.Add(Rifle);

	FGZWeaponState Shotgun;
	Shotgun.Type = EGZWeaponType::Shotgun;
	Shotgun.CurrentAmmo = 8;
	Shotgun.MaxAmmo = 8;
	Shotgun.ReserveAmmo = 40;
	Shotgun.FireRate = 0.6f;
	Shotgun.ReloadTime = 3.0f;
	Shotgun.Damage = 15.0f;
	Shotgun.Range = 3000.0f;
	WeaponInventory.Add(Shotgun);

	FGZWeaponState Sniper;
	Sniper.Type = EGZWeaponType::Sniper;
	Sniper.CurrentAmmo = 5;
	Sniper.MaxAmmo = 5;
	Sniper.ReserveAmmo = 25;
	Sniper.FireRate = 1.2f;
	Sniper.ReloadTime = 3.5f;
	Sniper.Damage = 100.0f;
	Sniper.Range = 50000.0f;
	WeaponInventory.Add(Sniper);

	SwitchWeapon(EGZWeaponType::Fist);
}

void AGZPlayerController::TakeDamage(float DamageAmount, AActor* DamageCauser)
{
	if (Armor > 0.0f)
	{
		float ArmorAbsorb = FMath::Min(Armor, DamageAmount * 0.5f);
		Armor -= ArmorAbsorb;
		DamageAmount -= ArmorAbsorb;
	}
	Health = FMath::Max(0.0f, Health - DamageAmount);

	if (Health <= 0.0f)
	{
		UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("Player has died."));
	}
}

void AGZPlayerController::Heal(float HealAmount)
{
	Health = FMath::Min(MaxHealth, Health + HealAmount);
}

void AGZPlayerController::AddMoney(int32 Amount)
{
	Money += Amount;
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Added %d money. Total: %d"), Amount, Money);
}

bool AGZPlayerController::SpendMoney(int32 Amount)
{
	if (Money < Amount) return false;
	Money -= Amount;
	return true;
}

void AGZPlayerController::SetWantedLevel(int32 NewLevel)
{
	WantedLevel = FMath::Clamp(NewLevel, 0, 5);
	WantedDecayTimer = 0.0f;
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Wanted level set to %d"), WantedLevel);
}

void AGZPlayerController::SwitchWeapon(EGZWeaponType NewWeapon)
{
	if (CurrentWeapon.bIsReloading) return;

	for (FGZWeaponState& Weapon : WeaponInventory)
	{
		if (Weapon.Type == NewWeapon)
		{
			CurrentWeapon = Weapon;
			UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Switched to weapon: %d"), (int32)NewWeapon);
			return;
		}
	}
}

void AGZPlayerController::FireWeapon()
{
	if (CurrentWeapon.bIsReloading) return;
	if (CurrentWeapon.Type == EGZWeaponType::Fist) return;

	if (CurrentWeapon.CurrentAmmo <= 0)
	{
		ReloadWeapon();
		return;
	}

	CurrentWeapon.CurrentAmmo--;

	APawn* MyPawn = GetPawn();
	if (!MyPawn) return;

	FVector Start = MyPawn->GetActorLocation();
	FVector Forward = MyPawn->GetActorForwardVector();
	FVector End = Start + Forward * CurrentWeapon.Range;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MyPawn);
	Params.bTraceComplex = true;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel1, Params))
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			HitActor->TakeDamage(CurrentWeapon.Damage, FDamageEvent(), this, MyPawn);
		}
	}
}

void AGZPlayerController::ReloadWeapon()
{
	if (CurrentWeapon.Type == EGZWeaponType::Fist) return;
	if (CurrentWeapon.bIsReloading) return;
	if (CurrentWeapon.CurrentAmmo >= CurrentWeapon.MaxAmmo) return;
	if (CurrentWeapon.ReserveAmmo <= 0) return;

	CurrentWeapon.bIsReloading = true;
	CurrentWeapon.ReloadProgress = 0.0f;
}

void AGZPlayerController::EnterVehicle(AActor* Vehicle)
{
	if (!Vehicle || bInVehicle) return;
	CurrentVehicle = Vehicle;
	bInVehicle = true;
}

void AGZPlayerController::ExitVehicle()
{
	if (!bInVehicle) return;
	CurrentVehicle = nullptr;
	bInVehicle = false;
}

void AGZPlayerController::DetectAndUpdateLightingZone()
{
	if (!CachedGameMode) return;

	APawn* MyPawn = GetPawn();
	if (!MyPawn) return;

	FVector Pos = MyPawn->GetActorLocation();
	EGZLightingZone DetectedZone = EGZLightingZone::OutdoorStreet;

	if (Pos.Z < -50.0f)
	{
		DetectedZone = EGZLightingZone::Tunnel;
	}
	else if (Pos.Z > 50.0f && Pos.Z < 200.0f)
	{
		DetectedZone = EGZLightingZone::Indoor;
	}
	else if (Pos.Z < 10.0f)
	{
		DetectedZone = EGZLightingZone::RiverSurface;
	}

	CachedGameMode->SetLightingZone(DetectedZone);
}

void AGZPlayerController::UpdateWantedDecay(float DeltaSeconds)
{
	if (WantedLevel <= 0) return;

	WantedDecayTimer += DeltaSeconds;
	if (WantedDecayTimer >= WantedDecayTime)
	{
		WantedLevel = FMath::Max(0, WantedLevel - 1);
		WantedDecayTimer = 0.0f;
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Wanted level decayed to %d"), WantedLevel);
	}
}

void AGZPlayerController::UpdateReload(float DeltaSeconds)
{
	if (!CurrentWeapon.bIsReloading) return;

	CurrentWeapon.ReloadProgress += DeltaSeconds / CurrentWeapon.ReloadTime;
	if (CurrentWeapon.ReloadProgress >= 1.0f)
	{
		CurrentWeapon.ReloadProgress = 0.0f;
		CurrentWeapon.bIsReloading = false;

		int32 Needed = CurrentWeapon.MaxAmmo - CurrentWeapon.CurrentAmmo;
		int32 Transfer = FMath::Min(Needed, CurrentWeapon.ReserveAmmo);
		CurrentWeapon.CurrentAmmo += Transfer;
		CurrentWeapon.ReserveAmmo -= Transfer;
	}
}

void AGZPlayerController::MoveForward(float Value)
{
	if (!bInVehicle)
	{
		if (APawn* MyPawn = GetPawn())
		{
			MyPawn->AddMovementInput(MyPawn->GetActorForwardVector(), Value);
		}
	}
}

void AGZPlayerController::MoveRight(float Value)
{
	if (!bInVehicle)
	{
		if (APawn* MyPawn = GetPawn())
		{
			MyPawn->AddMovementInput(MyPawn->GetActorRightVector(), Value);
		}
	}
}

void AGZPlayerController::Turn(float Value)
{
	AddYawInput(Value);
}

void AGZPlayerController::LookUp(float Value)
{
	AddPitchInput(Value);
}

void AGZPlayerController::StartSprint()
{
	if (ACharacter* Char = Cast<ACharacter>(GetPawn()))
	{
		Char->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
}

void AGZPlayerController::StopSprint()
{
	if (ACharacter* Char = Cast<ACharacter>(GetPawn()))
	{
		Char->GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}
}

void AGZPlayerController::StartCrouch()
{
	if (ACharacter* Char = Cast<ACharacter>(GetPawn()))
	{
		Char->Crouch();
	}
}

void AGZPlayerController::StopCrouch()
{
	if (ACharacter* Char = Cast<ACharacter>(GetPawn()))
	{
		Char->UnCrouch();
	}
}

void AGZPlayerController::FireAction()
{
	FireWeapon();
}

void AGZPlayerController::ReloadAction()
{
	ReloadWeapon();
}

void AGZPlayerController::SwitchWeaponAction(int32 WeaponIndex)
{
	if (WeaponIndex >= 0 && WeaponIndex < WeaponInventory.Num())
	{
		SwitchWeapon(WeaponInventory[WeaponIndex].Type);
	}
}