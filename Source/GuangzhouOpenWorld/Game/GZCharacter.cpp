#include "GZCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AGZCharacter::AGZCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);
    bUseControllerRotationYaw = false;
    // Third person camera
    USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 400.0f;
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 5.0f;
    UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;
}

void AGZCharacter::BeginPlay() { Super::BeginPlay(); }
void AGZCharacter::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
void AGZCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis("MoveForward", this, &AGZCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AGZCharacter::MoveRight);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("LookRight", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, [this](){ GetCharacterMovement()->MaxWalkSpeed = SprintSpeed; });
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, [this](){ GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; });
}