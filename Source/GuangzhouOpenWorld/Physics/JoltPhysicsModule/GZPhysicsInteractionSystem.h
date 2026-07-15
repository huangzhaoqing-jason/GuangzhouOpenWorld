#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZPhysicsInteractionSystem.generated.h"

UENUM(BlueprintType)
enum class EGZInteractionState : uint8
{
	Free     UMETA(DisplayName = "Free"),
	Held     UMETA(DisplayName = "Held"),
	Thrown   UMETA(DisplayName = "Thrown"),
	Placed   UMETA(DisplayName = "Placed"),
};

USTRUCT()
struct FGZInteractableObject
{
	GENERATED_BODY()

	UPROPERTY()
	int32 ObjectId = 0;

	UPROPERTY()
	float Mass = 1.0f;

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY()
	FVector AngularVelocity = FVector::ZeroVector;

	UPROPERTY()
	EGZInteractionState State = EGZInteractionState::Free;

	UPROPERTY()
	int32 HolderId = INDEX_NONE;

	UPROPERTY()
	float Restitution = 0.35f;

	UPROPERTY()
	float Friction = 0.5f;

	UPROPERTY()
	float CollisionRadius = 50.0f;

	UPROPERTY()
	bool bCanTriggerChain = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectChainCollision, int32, SourceObjectId, int32, HitObjectId);

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZPhysicsInteractionSystem : public UObject
{
	GENERATED_BODY()

public:
	UGZPhysicsInteractionSystem();

	UFUNCTION(BlueprintCallable)
	void Initialize(float InGravity = -981.0f);

	UFUNCTION(BlueprintCallable)
	int32 RegisterObject(float Mass, FVector Location, float CollisionRadius, bool bCanTriggerChain);

	UFUNCTION(BlueprintCallable)
	void UnregisterObject(int32 ObjectId);

	UFUNCTION(BlueprintCallable)
	bool StartGrab(int32 ObjectId, int32 HolderId);

	UFUNCTION(BlueprintCallable)
	void UpdateGrabTarget(int32 ObjectId, FVector TargetLocation, FVector TargetVelocity);

	UFUNCTION(BlueprintCallable)
	void ReleaseThrow(int32 ObjectId, FVector ThrowDirection, float ThrowForce);

	UFUNCTION(BlueprintCallable)
	void ReleasePlace(int32 ObjectId, FVector PlaceLocation);

	UFUNCTION(BlueprintCallable)
	void TickObjects(float DeltaSeconds);

	UFUNCTION(BlueprintPure)
	const TArray<FGZInteractableObject>& GetObjects() const { return Objects; }

	UFUNCTION(BlueprintPure)
	FGZInteractableObject GetObjectState(int32 ObjectId) const;

	UPROPERTY(BlueprintAssignable)
	FOnObjectChainCollision OnObjectChainCollision;

private:
	FGZInteractableObject* FindObject(int32 ObjectId);

	const FGZInteractableObject* FindObjectRead(int32 ObjectId) const;

	void ApplyGravityAndMovement(FGZInteractableObject& Object, float DeltaSeconds);

	void ResolveGroundAndChainCollisions(FGZInteractableObject& Object, float DeltaSeconds);

	FVector ComputeThrowImpulse(float Mass, FVector Direction, float Force) const;

	UPROPERTY()
	TArray<FGZInteractableObject> Objects;

	float Gravity = -981.0f;
	int32 NextObjectId = 1;
};
