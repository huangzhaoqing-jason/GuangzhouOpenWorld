#include "Game/Gameplay/GZSurfacePhysics.h"

UGZSurfacePhysics::UGZSurfacePhysics()
{
}

void UGZSurfacePhysics::Initialize()
{
	Profiles.Empty();
	RegisterDefaultProfiles();
}

float UGZSurfacePhysics::GetFriction(EGZGuangzhouSurfaceType Type, float Wetness01, float PuddleDepthCm) const
{
	const FGZSurfaceProfile* Profile = FindProfile(Type);
	if (!Profile)
	{
		return 0.8f;
	}

	Wetness01 = FMath::Clamp(Wetness01, 0.0f, 1.0f);
	const float WetFriction = FMath::Lerp(1.0f, Profile->WetFrictionMultiplier, Wetness01);

	const float EffectiveDepth = FMath::Clamp(PuddleDepthCm, 0.0f, Profile->MaxPuddleDepth);
	const float PuddleFactor = 1.0f - (EffectiveDepth / Profile->MaxPuddleDepth) * Profile->PuddleSensitivity;

	return Profile->BaseFriction * WetFriction * FMath::Max(PuddleFactor, 0.2f);
}

float UGZSurfacePhysics::GetSpeedPenalty(EGZGuangzhouSurfaceType Type, float PuddleDepthCm) const
{
	const FGZSurfaceProfile* Profile = FindProfile(Type);
	if (!Profile)
	{
		return 0.0f;
	}

	const float EffectiveDepth = FMath::Clamp(PuddleDepthCm, 0.0f, Profile->MaxPuddleDepth);
	return EffectiveDepth * Profile->SpeedPenaltyPerCm;
}

float UGZSurfacePhysics::GetNPCSpeedMultiplier(EGZGuangzhouSurfaceType Type, float Wetness01) const
{
	const FGZSurfaceProfile* Profile = FindProfile(Type);
	if (!Profile)
	{
		return 1.0f;
	}

	Wetness01 = FMath::Clamp(Wetness01, 0.0f, 1.0f);
	return FMath::Lerp(1.0f, Profile->NPCSpeedMultiplierWet, Wetness01);
}

float UGZSurfacePhysics::GetBrakingDistanceMultiplier(EGZGuangzhouSurfaceType Type, float Wetness01) const
{
	const FGZSurfaceProfile* Profile = FindProfile(Type);
	if (!Profile)
	{
		return 1.0f;
	}

	Wetness01 = FMath::Clamp(Wetness01, 0.0f, 1.0f);
	return FMath::Lerp(1.0f, Profile->BrakingDistanceMultiplierWet, Wetness01);
}

float UGZSurfacePhysics::GetMaxPuddleDepth(EGZGuangzhouSurfaceType Type) const
{
	const FGZSurfaceProfile* Profile = FindProfile(Type);
	if (!Profile)
	{
		return 15.0f;
	}
	return Profile->MaxPuddleDepth;
}

void UGZSurfacePhysics::RegisterDefaultProfiles()
{
	auto AddProfile = [&](EGZGuangzhouSurfaceType Type, float BaseFriction, float WetMultiplier,
		float PuddleSensitivity, float MaxDepth, float PenaltyPerCm,
		float NPCMultiplier, float BrakingMultiplier)
	{
		FGZSurfaceProfile Profile;
		Profile.SurfaceType = Type;
		Profile.BaseFriction = BaseFriction;
		Profile.WetFrictionMultiplier = WetMultiplier;
		Profile.PuddleSensitivity = PuddleSensitivity;
		Profile.MaxPuddleDepth = MaxDepth;
		Profile.SpeedPenaltyPerCm = PenaltyPerCm;
		Profile.NPCSpeedMultiplierWet = NPCMultiplier;
		Profile.BrakingDistanceMultiplierWet = BrakingMultiplier;
		Profiles.Add(Type, Profile);
	};

	// 广州典型路面：湿滑瓷砖摩擦极低，老城砖块排水差，窄巷沥青易积水
	AddProfile(EGZGuangzhouSurfaceType::WetCementTile,      0.90f, 0.25f, 0.80f, 12.0f, 0.035f, 0.60f, 2.40f);
	AddProfile(EGZGuangzhouSurfaceType::OldTownBrick,       0.80f, 0.40f, 0.65f, 18.0f, 0.025f, 0.70f, 1.90f);
	AddProfile(EGZGuangzhouSurfaceType::NarrowAlleyAsphalt, 0.85f, 0.35f, 0.75f, 20.0f, 0.030f, 0.65f, 2.10f);
	AddProfile(EGZGuangzhouSurfaceType::WornConcrete,       0.82f, 0.45f, 0.55f, 15.0f, 0.020f, 0.75f, 1.70f);
	AddProfile(EGZGuangzhouSurfaceType::SmoothConcrete,     0.92f, 0.50f, 0.50f, 10.0f, 0.015f, 0.80f, 1.50f);
	AddProfile(EGZGuangzhouSurfaceType::ManholeCover,       0.75f, 0.30f, 0.40f,  5.0f, 0.010f, 0.85f, 1.30f);
	AddProfile(EGZGuangzhouSurfaceType::DrainageGrate,      0.70f, 0.35f, 0.30f,  3.0f, 0.005f, 0.90f, 1.20f);
}

const FGZSurfaceProfile* UGZSurfacePhysics::FindProfile(EGZGuangzhouSurfaceType Type) const
{
	const FGZSurfaceProfile* Profile = Profiles.Find(Type);
	return Profile;
}
