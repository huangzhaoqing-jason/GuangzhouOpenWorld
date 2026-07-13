// GZCharacter.h - GTA-广州 Player Character
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GZCharacter.generated.h"

UCLASS()
class GUANGZHOUOPENWORLD_API AGZCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AGZCharacter();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditDefaultsOnly) float WalkSpeed = 600.0f;
    UPROPERTY(EditDefaultsOnly) float SprintSpeed = 1200.0f;
    UPROPERTY(EditDefaultsOnly) float CrouchSpeed = 300.0f;
};