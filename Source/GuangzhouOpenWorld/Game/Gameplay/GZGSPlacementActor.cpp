#include "Game/Gameplay/GZGSPlacementActor.h"
#include "Components/BoxComponent.h"
#include "NavModifierComponent.h"
#include "NavigationSystem.h"

AGZGSPlacementActor::AGZGSPlacementActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetCollisionProfileName(TEXT("BlockAll"));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCanEverAffectNavigation(false);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetCanEverAffectNavigation(false);

	NavModifier = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifier"));
	NavModifier->SetCanEverAffectNavigation(true);
}

void AGZGSPlacementActor::BeginPlay()
{
	Super::BeginPlay();
	RebuildComponentsFromAsset();
}

void AGZGSPlacementActor::RebuildComponentsFromAsset()
{
	if (!GSAsset)
	{
		return;
	}

	CollisionBox->SetBoxExtent(GSAsset->CollisionExtent);
	TriggerBox->SetBoxExtent(GSAsset->TriggerExtent);

	if (GSAsset->bAffectsNavigation && NavModifier)
	{
		NavModifier->SetAreaClass(GSAsset->NavAreaClass);
	}
	else if (NavModifier)
	{
		NavModifier->SetCanEverAffectNavigation(false);
	}

	// 触发一次导航重建
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		NavSys->UpdateComponentInNavOctree(*CollisionBox);
	}
}
