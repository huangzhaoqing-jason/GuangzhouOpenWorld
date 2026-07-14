#include "Game/Gameplay/GZProfessionSystem.h"
#include "GuangzhouOpenWorld.h"
#include "Game/GZGameMode.h"

UGZProfessionSystem::UGZProfessionSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGZProfessionSystem::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
}

void UGZProfessionSystem::Initialize()
{
	if (UWorld* World = GetWorld())
	{
		CachedGameMode = Cast<AGZGameMode>(World->GetAuthGameMode());
	}

	CurrentRole = EGZCharacterRole::UrbanExplorer;
}

void UGZProfessionSystem::SetPlayerProfession(EGZCharacterRole Role)
{
	CurrentRole = Role;
}

FProfessionConfig UGZProfessionSystem::GetProfessionConfig(EGZCharacterRole Role) const
{
	if (CachedGameMode.IsValid())
	{
		for (const FProfessionConfig& Config : CachedGameMode->ProfessionConfigs)
		{
			if (Config.Role == Role)
			{
				return Config;
			}
		}
	}

	FProfessionConfig DefaultConfig;
	DefaultConfig.Role = Role;
	return DefaultConfig;
}

float UGZProfessionSystem::GetNPCReactionModifier() const
{
	const FProfessionConfig Config = GetProfessionConfig(CurrentRole);
	return Config.NPCFriendlinessOffset;
}

bool UGZProfessionSystem::RunLayer1_APICompliance()
{
	return CachedGameMode.IsValid();
}

bool UGZProfessionSystem::RunLayer2_Syntax()
{
	return true;
}

bool UGZProfessionSystem::RunLayer3_Parameters()
{
	const FProfessionConfig Config = GetProfessionConfig(CurrentRole);
	return Config.ShopDiscount >= 0.0f && Config.EventRevealChance >= 0.0f;
}

bool UGZProfessionSystem::RunLayer4_FunctionalRules()
{
	return CurrentRole == EGZCharacterRole::DeliveryDriver
		|| CurrentRole == EGZCharacterRole::UrbanExplorer
		|| CurrentRole == EGZCharacterRole::PrivateDetective;
}
