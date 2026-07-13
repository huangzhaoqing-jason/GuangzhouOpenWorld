#include "Game/GZCharacter.h"
#include "GuangzhouOpenWorld.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AGZCharacter::AGZCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f / 60.0f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = SpringArmLength;
	SpringArm->SocketOffset = SpringArmSocketOffset;
	SpringArm->SetRelativeRotation(SpringArmRotation);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bDoCollisionTest = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 5.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 10.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->GravityScale = 1.75f;
}

void AGZCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AGZCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateIKTerrainAdaptation(DeltaSeconds);
}

void AGZCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGZCharacter::UpdateIKTerrainAdaptation(float DeltaSeconds)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement || !Movement->IsMovingOnGround()) return;

	FVector ActorLocation = GetActorLocation();
	FVector ActorForward = GetActorForwardVector();
	FVector ActorRight = GetActorRightVector();

	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();

	FVector LeftFootBase = ActorLocation - ActorRight * (CapsuleRadius * 0.5f) - FVector(0.0f, 0.0f, CapsuleHalfHeight);
	FVector RightFootBase = ActorLocation + ActorRight * (CapsuleRadius * 0.5f) - FVector(0.0f, 0.0f, CapsuleHalfHeight);

	FVector LeftFootTraceStart = LeftFootBase + FVector(0.0f, 0.0f, IKTraceDistance);
	FVector LeftFootTraceEnd = LeftFootBase - FVector(0.0f, 0.0f, IKTraceDistance);
	FVector RightFootTraceStart = RightFootBase + FVector(0.0f, 0.0f, IKTraceDistance);
	FVector RightFootTraceEnd = RightFootBase - FVector(0.0f, 0.0f, IKTraceDistance);

	FHitResult LeftHit, RightHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = false;

	float TargetLeftAlpha = 0.0f;
	float TargetRightAlpha = 0.0f;
	FVector TargetLeftOffset = FVector::ZeroVector;
	FVector TargetRightOffset = FVector::ZeroVector;

	if (GetWorld()->LineTraceSingleByChannel(LeftHit, LeftFootTraceStart, LeftFootTraceEnd, ECC_Visibility, Params))
	{
		TargetLeftAlpha = 1.0f;
		TargetLeftOffset = LeftHit.Location - LeftFootBase;
		TargetLeftOffset = GetActorTransform().InverseTransformVector(TargetLeftOffset);
	}

	if (GetWorld()->LineTraceSingleByChannel(RightHit, RightFootTraceStart, RightFootTraceEnd, ECC_Visibility, Params))
	{
		TargetRightAlpha = 1.0f;
		TargetRightOffset = RightHit.Location - RightFootBase;
		TargetRightOffset = GetActorTransform().InverseTransformVector(TargetRightOffset);
	}

	LeftFootIKAlpha = FMath::FInterpTo(LeftFootIKAlpha, TargetLeftAlpha, DeltaSeconds, IKInterpSpeed);
	RightFootIKAlpha = FMath::FInterpTo(RightFootIKAlpha, TargetRightAlpha, DeltaSeconds, IKInterpSpeed);
	LeftFootIKOffset = FMath::VInterpTo(LeftFootIKOffset, TargetLeftOffset, DeltaSeconds, IKInterpSpeed);
	RightFootIKOffset = FMath::VInterpTo(RightFootIKOffset, TargetRightOffset, DeltaSeconds, IKInterpSpeed);

	if (FMath::Abs(LeftFootIKAlpha - TargetLeftAlpha) > IKPrecision)
	{
		LeftFootIKAlpha = FMath::FInterpTo(LeftFootIKAlpha, TargetLeftAlpha, DeltaSeconds, IKInterpSpeed * 2.0f);
	}
	if (FMath::Abs(RightFootIKAlpha - TargetRightAlpha) > IKPrecision)
	{
		RightFootIKAlpha = FMath::FInterpTo(RightFootIKAlpha, TargetRightAlpha, DeltaSeconds, IKInterpSpeed * 2.0f);
	}
}