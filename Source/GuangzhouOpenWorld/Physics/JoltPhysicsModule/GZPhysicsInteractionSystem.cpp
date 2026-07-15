#include "Physics/JoltPhysicsModule/GZPhysicsInteractionSystem.h"

UGZPhysicsInteractionSystem::UGZPhysicsInteractionSystem()
{
}

void UGZPhysicsInteractionSystem::Initialize(float InGravity)
{
	Objects.Empty();
	Gravity = InGravity;
	NextObjectId = 1;
}

int32 UGZPhysicsInteractionSystem::RegisterObject(float Mass, FVector Location, float CollisionRadius, bool bCanTriggerChain)
{
	FGZInteractableObject Object;
	Object.ObjectId = NextObjectId++;
	Object.Mass = FMath::Max(Mass, 0.1f);
	Object.Location = Location;
	Object.CollisionRadius = FMath::Max(CollisionRadius, 1.0f);
	Object.bCanTriggerChain = bCanTriggerChain;
	Objects.Add(Object);
	return Object.ObjectId;
}

void UGZPhysicsInteractionSystem::UnregisterObject(int32 ObjectId)
{
	Objects.RemoveAll([ObjectId](const FGZInteractableObject& O) { return O.ObjectId == ObjectId; });
}

bool UGZPhysicsInteractionSystem::StartGrab(int32 ObjectId, int32 HolderId)
{
	FGZInteractableObject* Object = FindObject(ObjectId);
	if (!Object)
	{
		return false;
	}

	Object->State = EGZInteractionState::Held;
	Object->HolderId = HolderId;
	Object->Velocity = FVector::ZeroVector;
	return true;
}

void UGZPhysicsInteractionSystem::UpdateGrabTarget(int32 ObjectId, FVector TargetLocation, FVector TargetVelocity)
{
	FGZInteractableObject* Object = FindObject(ObjectId);
	if (Object && Object->State == EGZInteractionState::Held)
	{
		Object->Location = TargetLocation;
		Object->Velocity = TargetVelocity;
	}
}

void UGZPhysicsInteractionSystem::ReleaseThrow(int32 ObjectId, FVector ThrowDirection, float ThrowForce)
{
	FGZInteractableObject* Object = FindObject(ObjectId);
	if (!Object)
	{
		return;
	}

	Object->State = EGZInteractionState::Thrown;
	Object->HolderId = INDEX_NONE;
	Object->Velocity = ComputeThrowImpulse(Object->Mass, ThrowDirection.GetSafeNormal(), ThrowForce);
}

void UGZPhysicsInteractionSystem::ReleasePlace(int32 ObjectId, FVector PlaceLocation)
{
	FGZInteractableObject* Object = FindObject(ObjectId);
	if (!Object)
	{
		return;
	}

	Object->State = EGZInteractionState::Placed;
	Object->HolderId = INDEX_NONE;
	Object->Location = PlaceLocation;
	Object->Velocity = FVector::ZeroVector;
}

void UGZPhysicsInteractionSystem::TickObjects(float DeltaSeconds)
{
	DeltaSeconds = FMath::Clamp(DeltaSeconds, 0.0f, 0.05f);

	for (FGZInteractableObject& Object : Objects)
	{
		if (Object.State == EGZInteractionState::Held)
		{
			continue;
		}

		ApplyGravityAndMovement(Object, DeltaSeconds);
		ResolveGroundAndChainCollisions(Object, DeltaSeconds);
	}
}

FGZInteractableObject UGZPhysicsInteractionSystem::GetObjectState(int32 ObjectId) const
{
	const FGZInteractableObject* Object = FindObjectRead(ObjectId);
	return Object ? *Object : FGZInteractableObject();
}

FGZInteractableObject* UGZPhysicsInteractionSystem::FindObject(int32 ObjectId)
{
	return Objects.FindByPredicate([ObjectId](const FGZInteractableObject& O) { return O.ObjectId == ObjectId; });
}

const FGZInteractableObject* UGZPhysicsInteractionSystem::FindObjectRead(int32 ObjectId) const
{
	return Objects.FindByPredicate([ObjectId](const FGZInteractableObject& O) { return O.ObjectId == ObjectId; });
}

void UGZPhysicsInteractionSystem::ApplyGravityAndMovement(FGZInteractableObject& Object, float DeltaSeconds)
{
	if (Object.State == EGZInteractionState::Placed)
	{
		return;
	}

	Object.Velocity.Z += Gravity * DeltaSeconds;
	Object.Location += Object.Velocity * DeltaSeconds;

	// 空气阻力
	Object.Velocity *= FMath::Pow(0.999f, DeltaSeconds * 60.0f);
}

void UGZPhysicsInteractionSystem::ResolveGroundAndChainCollisions(FGZInteractableObject& Object, float DeltaSeconds)
{
	if (Object.State == EGZInteractionState::Placed)
	{
		return;
	}

	// 地面碰撞（简化地面为 Z=0）
	if (Object.Location.Z <= Object.CollisionRadius)
	{
		Object.Location.Z = Object.CollisionRadius;
		Object.Velocity.Z *= -Object.Restitution;
		Object.Velocity.X *= (1.0f - Object.Friction * DeltaSeconds);
		Object.Velocity.Y *= (1.0f - Object.Friction * DeltaSeconds);

		if (FMath::Abs(Object.Velocity.Z) < 50.0f)
		{
			Object.Velocity.Z = 0.0f;
		}

		if (Object.Velocity.SizeSquared2D() < FMath::Square(10.0f))
		{
			Object.State = EGZInteractionState::Free;
		}
	}

	// 连锁碰撞：对附近物体施加冲量
	if (!Object.bCanTriggerChain)
	{
		return;
	}

	const float TotalRadius = Object.CollisionRadius * 2.0f;
	for (FGZInteractableObject& Other : Objects)
	{
		if (Other.ObjectId == Object.ObjectId || Other.State == EGZInteractionState::Held)
		{
			continue;
		}

		const float Dist = FVector::Dist(Object.Location, Other.Location);
		if (Dist < TotalRadius + Other.CollisionRadius)
		{
			const FVector Dir = (Other.Location - Object.Location).GetSafeNormal();
			const float Speed = FMath::Max(Object.Velocity.Size(), 100.0f);
			const float ImpulseMagnitude = Speed * Object.Mass / FMath::Max(Other.Mass, 0.1f);
			const float Falloff = 1.0f - FMath::Clamp(Dist / (TotalRadius + Other.CollisionRadius), 0.0f, 1.0f);

			Other.Velocity += Dir * ImpulseMagnitude * Falloff * 0.3f;
			Other.State = EGZInteractionState::Thrown;

			OnObjectChainCollision.Broadcast(Object.ObjectId, Other.ObjectId);
		}
	}
}

FVector UGZPhysicsInteractionSystem::ComputeThrowImpulse(float Mass, FVector Direction, float Force) const
{
	// 力转换为速度：v = F / m，施加质量缩放使重物体初速更低
	const float Speed = Force / FMath::Max(Mass, 0.1f);
	return Direction * Speed;
}
