// GuangzhouOpenWorld.cpp
// 主模块实现 — 初始化所有子系统

#include "GuangzhouOpenWorld.h"
#include "Game/GZGameMode.h"
#include "Physics/GZVehiclePhysics.h"
#include "AI/GZNavigationSystem.h"
#include "Scene/GZWorldPartition.h"
#include "Audio/GZAudioManager.h"
#include "Network/GZNetworkManager.h"

DEFINE_LOG_CATEGORY(LogGZWorld);

void FGuangzhouOpenWorldModule::StartupModule()
{
    UE_LOG(LogGZWorld, Log, TEXT("GTA-广州 GuangzhouOpenWorld Module Started"));
    UE_LOG(LogGZWorld, Log, TEXT("Target: macOS Apple Silicon | Metal 4 | Nanite | Lumen | Jolt Physics"));
}

void FGuangzhouOpenWorldModule::ShutdownModule()
{
    UE_LOG(LogGZWorld, Log, TEXT("GTA-广州 Module Shutdown"));
}

IMPLEMENT_PRIMARY_GAME_MODULE(FGuangzhouOpenWorldModule, GuangzhouOpenWorld, "GuangzhouOpenWorld");