#include "GZJoltWindParams.h"

FZJoltWindObjectParams UGZJoltWindParams::GetParams(EJoltWindObjectType Type)
{
	FZJoltWindObjectParams P;
	P.ObjectType = Type;

	switch (Type)
	{
	case EJoltWindObjectType::MetalSign:
		P.MassKg = 80.0f;
		P.Stiffness = 25.0f;
		P.MinPeriod = 6.0f;
		P.MaxPeriod = 10.0f;
		P.MaxSwingAngle = 8.0f;
		P.SwingScale = 0.6f;
		break;
	case EJoltWindObjectType::ClothBanner:
		P.MassKg = 5.0f;
		P.Stiffness = 2.0f;
		P.MinPeriod = 3.0f;
		P.MaxPeriod = 7.0f;
		P.MaxSwingAngle = 15.0f;
		P.SwingScale = 1.4f;
		break;
	case EJoltWindObjectType::ThickBranch:
		P.MassKg = 40.0f;
		P.Stiffness = 6.0f;
		P.MinPeriod = 8.0f;
		P.MaxPeriod = 12.0f;
		P.MaxSwingAngle = 6.0f;
		P.SwingScale = 1.0f;
		break;
	case EJoltWindObjectType::ThinBranch:
		P.MassKg = 2.0f;
		P.Stiffness = 1.5f;
		P.MinPeriod = 2.0f;
		P.MaxPeriod = 4.0f;
		P.MaxSwingAngle = 12.0f;
		P.SwingScale = 1.6f;
		break;
	case EJoltWindObjectType::Leaf:
		P.MassKg = 0.1f;
		P.Stiffness = 0.5f;
		P.MinPeriod = 0.5f;
		P.MaxPeriod = 2.0f;
		P.MaxSwingAngle = 20.0f;
		P.SwingScale = 2.0f;
		break;
	case EJoltWindObjectType::Conifer:
		P.MassKg = 30.0f;
		P.Stiffness = 7.0f;
		P.MinPeriod = 7.0f;
		P.MaxPeriod = 11.0f;
		P.MaxSwingAngle = 5.0f;
		P.SwingScale = 0.85f;
		break;
	case EJoltWindObjectType::TallTreeCrown:
		P.MassKg = 15.0f;
		P.Stiffness = 2.0f;
		P.MinPeriod = 4.0f;
		P.MaxPeriod = 8.0f;
		P.MaxSwingAngle = 10.0f;
		P.SwingScale = 1.3f;
		break;
	case EJoltWindObjectType::LowShrub:
		P.MassKg = 8.0f;
		P.Stiffness = 10.0f;
		P.MinPeriod = 5.0f;
		P.MaxPeriod = 9.0f;
		P.MaxSwingAngle = 4.0f;
		P.SwingScale = 0.7f;
		break;
	}

	return P;
}

float UGZJoltWindParams::ComputeGustIntervalSeconds()
{
	return FMath::RandRange(15.0f, 45.0f);
}

float UGZJoltWindParams::ClampSwingAngle(EJoltWindObjectType Type, float Angle)
{
	FZJoltWindObjectParams P = GetParams(Type);
	return FMath::Clamp(Angle, -P.MaxSwingAngle, P.MaxSwingAngle);
}
