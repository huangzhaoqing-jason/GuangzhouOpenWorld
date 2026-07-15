#include "Game/Gameplay/GZP1GameMode.h"
#include "Game/Gameplay/GZSurfacePhysics.h"
#include "AI/RecastMassAI/GZTrafficConflictManager.h"
#include "AI/RecastMassAI/GZNPCMemoryTagSystem.h"
#include "Game/Gameplay/GZVerticalPathSystem.h"
#include "Physics/JoltPhysicsModule/GZPhysicsInteractionSystem.h"
#include "UI/LiquidGlass/GZLiquidGlassPresenter.h"
#include "Scene/GSRendering/GZGSEnvironmentBridge.h"
#include "Scene/GSRendering/GZGSRendererIntegration.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"

AGZP1GameMode::AGZP1GameMode()
{
}

void AGZP1GameMode::BeginPlay()
{
	Super::BeginPlay();

	if (bApplyPerformanceFallback)
	{
		ApplyPerformanceFallback();
	}

	InitializeP1Systems();
	BuildP1SliceData();
	BindGSToPlayerCamera();
}

void AGZP1GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentTime += DeltaSeconds;

	if (TrafficManager)
	{
		TrafficManager->TickTraffic(DeltaSeconds, SurfacePhysics, Wetness01);
	}

	if (NPCMemory)
	{
		NPCMemory->TickMemory(DeltaSeconds, CurrentTime);
	}

	if (PhysicsInteraction)
	{
		PhysicsInteraction->TickObjects(DeltaSeconds);
	}

	// 同步环境变量到 GS 渲染桥：湿度、风速、光照、降雨
	if (EnvironmentBridge)
	{
		const float WindSpeed = 2.0f + Wetness01 * 6.0f;
		const float LightIntensity = FMath::Lerp(1.0f, 0.55f, Wetness01);
		const float RainIntensity = Wetness01 > 0.6f ? (Wetness01 - 0.6f) / 0.4f : 0.0f;
		const float TimeOfDay = 12.0f;
		EnvironmentBridge->SetEnvironment(Wetness01, WindSpeed, LightIntensity, RainIntensity, TimeOfDay);

		if (GSRenderer)
		{
			GSRenderer->SetEnvironmentParameters(Wetness01, WindSpeed, LightIntensity, RainIntensity);
		}
	}

	TickMission(DeltaSeconds);
	UpdatePresenter(DeltaSeconds);
}

void AGZP1GameMode::InitializeP1Systems()
{
	SurfacePhysics = NewObject<UGZSurfacePhysics>(this);
	SurfacePhysics->Initialize();

	TrafficManager = NewObject<UGZTrafficConflictManager>(this);
	TrafficManager->Initialize();

	NPCMemory = NewObject<UGZNPCMemoryTagSystem>(this);
	NPCMemory->Initialize();

	VerticalPath = NewObject<UGZVerticalPathSystem>(this);
	VerticalPath->Initialize();

	PhysicsInteraction = NewObject<UGZPhysicsInteractionSystem>(this);
	PhysicsInteraction->Initialize(-981.0f);

	Presenter = NewObject<UGZLiquidGlassPresenter>(this);
	Presenter->Initialize();

	EnvironmentBridge = NewObject<UGZGSEnvironmentBridge>(this);
	EnvironmentBridge->Initialize();

	GSRenderer = NewObject<UGZGSRendererIntegration>(this);
	GSRenderer->Initialize();
	GSRenderer->LoadNativePlugin();
}

void AGZP1GameMode::ApplyPerformanceFallback()
{
	if (!GEngine || !GetWorld())
	{
		return;
	}

	static const TArray<FString> Commands =
	{
		TEXT("r.Lumen.Reflections.Allow 0"),
		TEXT("r.Lumen.GlobalIllumination.Allow 0"),
		TEXT("r.Lumen.DiffuseIndirect.Allow 0"),
		TEXT("r.Nanite.MaxPixelsPerEdge 32"),
		TEXT("r.RayTracing.Shadows 0"),
		TEXT("r.RayTracing.SkyLight 0"),
		TEXT("r.VolumetricCloud 0"),
		TEXT("r.VolumetricFog 0"),
		TEXT("r.MotionBlurQuality 0"),
		TEXT("r.SceneColorFringeQuality 0"),
		TEXT("sg.ShadowQuality 2"),
		TEXT("sg.TextureQuality 2"),
		TEXT("sg.EffectsQuality 2"),
		TEXT("r.ScreenPercentage 85"),
		TEXT("r.DynamicRes.MinScreenPercentage 85"),
		TEXT("r.DynamicRes.MaxScreenPercentage 100"),
		TEXT("r.ViewDistanceScale 0.8"),
	};

	for (const FString& Cmd : Commands)
	{
		GEngine->Exec(GetWorld(), *Cmd);
	}
}

void AGZP1GameMode::BuildP1SliceData()
{
	if (!SurfacePhysics || !TrafficManager || !NPCMemory || !VerticalPath || !PhysicsInteraction)
	{
		return;
	}

	// 天河 CBD 车流：湿滑瓷砖路面，密度较高
	for (int32 i = 0; i < 8; ++i)
	{
		const FVector Loc = FVector(1000.0f + i * 600.0f, 0.0f, 0.0f);
		TrafficManager->RegisterVehicle(EGZCityDistrict::TianheCBD, Loc, FVector::ForwardVector, 700.0f, EGZGuangzhouSurfaceType::SmoothConcrete);
	}

	// 越秀老城窄巷车流：老城砖 + 湿滑
	for (int32 i = 0; i < 5; ++i)
	{
		const FVector Loc = FVector(8000.0f + i * 350.0f, 4000.0f, 0.0f);
		TrafficManager->RegisterVehicle(EGZCityDistrict::YuexiuOldTown, Loc, FVector::ForwardVector, 300.0f, EGZGuangzhouSurfaceType::OldTownBrick);
	}

	// NPC：摊主 + 路人
	MissionNPCId = 1001;
	NPCMemory->RegisterNPC(MissionNPCId, MissionArcadeCenter);
	NPCMemory->RegisterNPC(1002, MissionArcadeCenter + FVector(300.0f, 0.0f, 0.0f));
	NPCMemory->RegisterNPC(1003, MissionArcadeCenter + FVector(-300.0f, 200.0f, 0.0f));

	// 垂直路径：街道 → 骑楼遮蔽 → 二楼阳台 → 屋顶
	const int32 StreetNode = VerticalPath->RegisterNode(MissionArcadeCenter + FVector(0.0f, -500.0f, 0.0f), EGZVerticalLayer::Street, false, false, 1.0f);
	const int32 ArcadeNode = VerticalPath->RegisterNode(MissionArcadeCenter + FVector(0.0f, 0.0f, 0.0f), EGZVerticalLayer::Arcade, true, false, 1.2f);
	const int32 BalconyNode = VerticalPath->RegisterNode(MissionArcadeCenter + FVector(0.0f, 200.0f, 300.0f), EGZVerticalLayer::Balcony, true, true, 1.5f);
	const int32 RooftopNode = VerticalPath->RegisterNode(MissionArcadeCenter + FVector(0.0f, 0.0f, 600.0f), EGZVerticalLayer::Rooftop, false, true, 2.0f);

	VerticalPath->ConnectNodes(StreetNode, ArcadeNode);
	VerticalPath->ConnectNodes(ArcadeNode, BalconyNode);
	VerticalPath->ConnectNodes(BalconyNode, RooftopNode);

	// 风/光引导：从起点指向驾驶目标
	VerticalPath->AddGuidanceCue(EGZGuidanceType::Wind, FVector::ZeroVector, (MissionDriveTarget - FVector::ZeroVector).GetSafeNormal(), 25000.0f);

	// 可互动物件：一个木箱（轻）和一个货箱（重）
	PhysicsInteraction->RegisterObject(5.0f, MissionShopLocation + FVector(0.0f, 0.0f, 50.0f), 40.0f, true);
	PhysicsInteraction->RegisterObject(25.0f, MissionShopLocation + FVector(150.0f, 0.0f, 50.0f), 60.0f, true);
}

void AGZP1GameMode::TickMission(float DeltaSeconds)
{
	MissionTimer += DeltaSeconds;

	const FVector PlayerLoc = GetPlayerPawnLocation();

	switch (MissionState)
	{
	case EP1MissionState::Intro:
		if (MissionTimer > 1.5f)
		{
			MissionState = EP1MissionState::Drive;
			MissionTimer = 0.0f;
		}
		break;

	case EP1MissionState::Drive:
		if (FVector::Dist2D(PlayerLoc, MissionDriveTarget) < DriveAcceptRadius)
		{
			MissionState = EP1MissionState::Explore;
			MissionTimer = 0.0f;
		}
		break;

	case EP1MissionState::Explore:
		if (VerticalPath)
		{
			const int32 NearestCover = VerticalPath->FindNearestNode(PlayerLoc, ArcadeAcceptRadius, EGZVerticalLayer::Arcade);
			if (NearestCover != INDEX_NONE)
			{
				MissionState = EP1MissionState::Conflict;
				MissionTimer = 0.0f;
			}
		}
		break;

	case EP1MissionState::Conflict:
		if (NPCMemory && MissionNPCId != INDEX_NONE)
		{
			NPCMemory->AddTag(MissionNPCId, EGZNPCConflictTag::VendorDisturbed, 35.0f, CurrentTime);
			if (NPCMemory->GetNPCStage(MissionNPCId) >= EGZNPCConflictStage::Angry)
			{
				MissionState = EP1MissionState::Interact;
				MissionTimer = 0.0f;
			}
		}
		break;

	case EP1MissionState::Interact:
		if (MissionObjectId == INDEX_NONE && PhysicsInteraction)
		{
			MissionObjectId = PhysicsInteraction->RegisterObject(8.0f, PlayerLoc + FVector(100.0f, 0.0f, 50.0f), 45.0f, true);
		}

		if (PhysicsInteraction)
		{
			const FGZInteractableObject State = PhysicsInteraction->GetObjectState(MissionObjectId);
			if (State.State == EGZInteractionState::Thrown)
			{
				bMissionObjectThrown = true;
			}
		}

		if (bMissionObjectThrown)
		{
			MissionState = EP1MissionState::Complete;
			MissionTimer = 0.0f;
		}
		break;

	case EP1MissionState::Complete:
		break;
	}
}

void AGZP1GameMode::UpdatePresenter(float DeltaSeconds)
{
	if (!Presenter)
	{
		return;
	}

	const FVector PlayerLoc = GetPlayerPawnLocation();

	// 速度：从玩家 Pawn 速度读取，无 Pawn 时归零
	float Speed = 0.0f;
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			Speed = Pawn->GetVelocity().Size();
		}
	}
	Presenter->SetSpeed(Speed);

	// 引导方向：当前任务目标
	FVector GuideDir = FVector::ZeroVector;
	switch (MissionState)
	{
	case EP1MissionState::Drive:
		GuideDir = (MissionDriveTarget - PlayerLoc).GetSafeNormal();
		break;
	case EP1MissionState::Explore:
	case EP1MissionState::Conflict:
		GuideDir = (MissionArcadeCenter - PlayerLoc).GetSafeNormal();
		break;
	case EP1MissionState::Interact:
		GuideDir = (MissionShopLocation - PlayerLoc).GetSafeNormal();
		break;
	default:
		break;
	}

	if (!GuideDir.IsNearlyZero())
	{
		Presenter->SetGuidanceDirection(GuideDir);
	}
	else
	{
		Presenter->ClearGuidance();
	}

	// 冲突阶段
	if (NPCMemory && MissionNPCId != INDEX_NONE)
	{
		Presenter->SetConflictStage(MissionNPCId, NPCMemory->GetNPCStage(MissionNPCId));
	}

	// 交互提示
	if (MissionState == EP1MissionState::Interact && !bMissionObjectThrown)
	{
		Presenter->SetInteractionPrompt(FText::FromString(TEXT("拾取并投掷木箱")));
	}
	else
	{
		Presenter->ClearInteractionPrompt();
	}

	// 湿滑警告：低速在湿滑路面上打滑的简化提示
	if (Speed > 100.0f && Wetness01 > 0.3f)
	{
		Presenter->SetSkidWarning(true);
	}
	else
	{
		Presenter->SetSkidWarning(false);
	}
}

void AGZP1GameMode::BindGSToPlayerCamera()
{
	if (!GSRenderer)
	{
		return;
	}

	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	if (!Pawn)
	{
		return;
	}

	UCameraComponent* Camera = Pawn->FindComponentByClass<UCameraComponent>();
	if (!Camera)
	{
		return;
	}

	GSRenderer->SetupPostProcessBridge(Camera);
	GSRenderer->ApplySharedPostProcess(Camera);
}

FVector AGZP1GameMode::GetPlayerPawnLocation() const
{
	if (const UWorld* World = GetWorld())
	{
		if (const APlayerController* PC = World->GetFirstPlayerController())
		{
			if (const APawn* Pawn = PC->GetPawn())
			{
				return Pawn->GetActorLocation();
			}
		}
	}
	return FVector::ZeroVector;
}
