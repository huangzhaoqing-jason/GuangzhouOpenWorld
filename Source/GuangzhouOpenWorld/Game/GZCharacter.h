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
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure, Category = "Movement")
	float GetWalkSpeed() const { return WalkSpeed; }

	UFUNCTION(BlueprintPure, Category = "Movement")
	float GetSprintSpeed() const { return SprintSpeed; }

	UFUNCTION(BlueprintPure, Category = "Movement")
	float GetCrouchSpeed() const { return CrouchSpeed; }

protected:
	void UpdateIKTerrainAdaptation(float DeltaSeconds);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	class USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	class UCameraComponent* Camera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float IKTraceDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float IKInterpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float IKPrecision = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	FVector LeftFootIKOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	FVector RightFootIKOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float LeftFootIKAlpha = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float RightFootIKAlpha = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float SpringArmLength = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector SpringArmSocketOffset = FVector(0.0f, 0.0f, 70.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FRotator SpringArmRotation = FRotator(-10.0f, 0.0f, 0.0f);
};