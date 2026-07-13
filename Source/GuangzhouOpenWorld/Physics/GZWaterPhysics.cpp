#include "Physics/GZWaterPhysics.h"
#include "GuangzhouOpenWorld.h"
#include "Math/UnrealMathUtility.h"

UGZWaterPhysics::UGZWaterPhysics()
{
}

void UGZWaterPhysics::Initialize(int32 InGridSize, float InCellSize)
{
	GridSize = InGridSize;
	CellSize = InCellSize;
	TotalWidth = GridSize * CellSize;

	GridVertices.Empty();
	GridVertices.Reserve(GridSize * GridSize);

	float HalfWidth = TotalWidth * 0.5f;
	for (int32 Y = 0; Y < GridSize; ++Y)
	{
		for (int32 X = 0; X < GridSize; ++X)
		{
			FGZWaterGridVertex V;
			V.Position = FVector(X * CellSize - HalfWidth, Y * CellSize - HalfWidth, 0.0f);
			V.Height = WaterBaseHeight;
			V.Normal = FVector::UpVector;
			V.Pressure = 0.0f;
			V.Density = WaterDensity;
			GridVertices.Add(V);
		}
	}

	Waves.Empty();
	FGZGerstnerWave W1;
	W1.Amplitude = 0.8f;
	W1.Frequency = 0.5f;
	W1.Direction = FVector2D(0.0f, 1.0f);
	W1.Steepness = 0.3f;
	Waves.Add(W1);

	FGZGerstnerWave W2;
	W2.Amplitude = 0.4f;
	W2.Frequency = 1.2f;
	W2.Direction = FVector2D(0.3f, 0.8f);
	W2.Steepness = 0.2f;
	Waves.Add(W2);

	FGZGerstnerWave W3;
	W3.Amplitude = 0.2f;
	W3.Frequency = 2.0f;
	W3.Direction = FVector2D(-0.1f, 1.0f);
	W3.Steepness = 0.15f;
	Waves.Add(W3);

	FGZGerstnerWave W4;
	W4.Amplitude = 0.15f;
	W4.Frequency = 3.5f;
	W4.Direction = FVector2D(0.1f, 0.9f);
	W4.Steepness = 0.1f;
	Waves.Add(W4);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Water physics initialized: %dx%d grid, %.0fm x %.0fm, %d waves"),
		GridSize, GridSize, TotalWidth, TotalWidth, Waves.Num());
}

void UGZWaterPhysics::Simulate(float DeltaTime)
{
	SimulationTime += DeltaTime;

	UpdateGerstnerWaves(DeltaTime);
	UpdateTidalCycle(DeltaTime);
	UpdateRipples(DeltaTime);
	UpdateSPHGrid(DeltaTime);
	RecalculateNormals();
}

void UGZWaterPhysics::AddRipple(const FVector& ImpactPoint, float ImpactForce)
{
	FGZRipple Ripple;
	Ripple.Center = ImpactPoint;
	Ripple.Amplitude = FMath::Clamp(ImpactForce * 0.001f, 0.1f, 3.0f);
	Ripple.Radius = 1.0f;
	Ripple.Lifetime = 0.0f;
	Ripple.MaxLifetime = 2.0f;
	Ripple.Speed = 2.0f + ImpactForce * 0.01f;
	ActiveRipples.Add(Ripple);
}

float UGZWaterPhysics::GetWaterHeight(const FVector& WorldPosition) const
{
	float X = WorldPosition.X;
	float Y = WorldPosition.Y;
	float Height = WaterBaseHeight + GetTidalOffset();

	for (const FGZGerstnerWave& Wave : Waves)
	{
		Height += EvaluateGerstnerWave(Wave, X, Y, SimulationTime);
	}

	for (const FGZRipple& Ripple : ActiveRipples)
	{
		float Dist = FVector::Dist2D(WorldPosition, Ripple.Center);
		float RippleHeight = Ripple.Amplitude * FMath::Exp(-Dist / (Ripple.Radius + 1.0f))
			* FMath::Cos(Dist * 0.5f - Ripple.Lifetime * 3.0f)
			* (1.0f - Ripple.Lifetime / Ripple.MaxLifetime);
		Height += RippleHeight;
	}

	return Height;
}

FVector UGZWaterPhysics::GetWaterNormal(const FVector& WorldPosition) const
{
	float HalfWidth = TotalWidth * 0.5f;
	float X = WorldPosition.X;
	float Y = WorldPosition.Y;

	int32 IX = FMath::Clamp(FMath::RoundToInt((X + HalfWidth) / CellSize), 0, GridSize - 1);
	int32 IY = FMath::Clamp(FMath::RoundToInt((Y + HalfWidth) / CellSize), 0, GridSize - 1);
	int32 Idx = IY * GridSize + IX;

	if (Idx >= 0 && Idx < GridVertices.Num())
	{
		return GridVertices[Idx].Normal;
	}
	return FVector::UpVector;
}

FVector UGZWaterPhysics::CalculateBuoyancy(const FVector& ObjectPosition, float ObjectRadius, float ObjectMass) const
{
	float WaterHeight = GetWaterHeight(ObjectPosition);
	float SubmergedDepth = WaterHeight - ObjectPosition.Z;

	if (SubmergedDepth <= 0.0f) return FVector::ZeroVector;

	float SubmergedVolume = PI * ObjectRadius * ObjectRadius * FMath::Min(SubmergedDepth, ObjectRadius * 2.0f);
	float BuoyancyForce = WaterDensity * Gravity * SubmergedVolume;
	float Weight = ObjectMass * Gravity;

	return FVector(0.0f, 0.0f, BuoyancyForce - Weight * 0.5f);
}

float UGZWaterPhysics::GetTidalOffset() const
{
	return TidalAmplitude * FMath::Sin(TidalPhase);
}

void UGZWaterPhysics::UpdateGerstnerWaves(float DeltaTime)
{
	float HalfWidth = TotalWidth * 0.5f;
	for (int32 Y = 0; Y < GridSize; ++Y)
	{
		for (int32 X = 0; X < GridSize; ++X)
		{
			int32 Idx = Y * GridSize + X;
			float WorldX = GridVertices[Idx].Position.X;
			float WorldY = GridVertices[Idx].Position.Y;

			float Height = WaterBaseHeight + GetTidalOffset();
			for (const FGZGerstnerWave& Wave : Waves)
			{
				Height += EvaluateGerstnerWave(Wave, WorldX, WorldY, SimulationTime);
			}
			GridVertices[Idx].Height = Height;
		}
	}
}

void UGZWaterPhysics::UpdateTidalCycle(float DeltaTime)
{
	TidalPhase += (2.0f * PI / TidalPeriod) * DeltaTime;
	if (TidalPhase > 2.0f * PI) TidalPhase -= 2.0f * PI;
}

void UGZWaterPhysics::UpdateRipples(float DeltaTime)
{
	for (int32 i = ActiveRipples.Num() - 1; i >= 0; --i)
	{
		ActiveRipples[i].Lifetime += DeltaTime;
		ActiveRipples[i].Radius += ActiveRipples[i].Speed * DeltaTime;

		if (ActiveRipples[i].Lifetime >= ActiveRipples[i].MaxLifetime)
		{
			ActiveRipples.RemoveAt(i);
		}
	}
}

void UGZWaterPhysics::UpdateSPHGrid(float DeltaTime)
{
	for (int32 i = 0; i < GridVertices.Num(); ++i)
	{
		GridVertices[i].Pressure = FMath::Max(0.0f, GridVertices[i].Height - WaterBaseHeight) * WaterDensity * Gravity;
		GridVertices[i].Density = WaterDensity + GridVertices[i].Pressure * 0.0001f;
	}
}

void UGZWaterPhysics::RecalculateNormals()
{
	for (int32 Y = 1; Y < GridSize - 1; ++Y)
	{
		for (int32 X = 1; X < GridSize - 1; ++X)
		{
			int32 Idx = Y * GridSize + X;
			float HLeft = GridVertices[Y * GridSize + (X - 1)].Height;
			float HRight = GridVertices[Y * GridSize + (X + 1)].Height;
			float HUp = GridVertices[(Y - 1) * GridSize + X].Height;
			float HDown = GridVertices[(Y + 1) * GridSize + X].Height;

			FVector Normal = FVector((HLeft - HRight) / (2.0f * CellSize), (HDown - HUp) / (2.0f * CellSize), 1.0f);
			Normal.Normalize();
			GridVertices[Idx].Normal = Normal;
		}
	}
}

float UGZWaterPhysics::EvaluateGerstnerWave(const FGZGerstnerWave& Wave, float X, float Y, float Time) const
{
	float DirX = Wave.Direction.X;
	float DirY = Wave.Direction.Y;
	float Dot = DirX * X + DirY * Y;
	float Phase = Wave.Phase + Wave.Frequency * Dot;
	return Wave.Amplitude * FMath::Sin(Phase + Time * Wave.Frequency * 2.0f);
}