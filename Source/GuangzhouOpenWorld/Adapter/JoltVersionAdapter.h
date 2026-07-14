#pragma once

// JoltVersionAdapter.h
// Jolt Physics v6.0.1 版本适配层
// 原则：保留全部原有类名、函数名、参数、宏，仅做 Mac 平台 GPU 接口屏蔽

#include "CoreMinimal.h"

#ifndef JOLT_VERSION_601
	#define JOLT_VERSION_601 1
#endif

// 保留 Jolt 核心类名/函数名别名，上层调用代码不变
#define JoltBody              FZJoltBodyAlias
#define JoltConstraint        FZJoltConstraintAlias
#define JoltPhysicsSystem     FZJoltPhysicsSystemAlias
#define JoltBodyInterface     FZJoltBodyInterfaceAlias

// Mac 平台仅屏蔽 v6 专属 GPU 加速接口；Windows 保留原版
#if PLATFORM_MAC
	#define JoltGPUInterface_Shutdown     (void)0
	#define JoltGPUInterface_Sync         (void)0
	#define JoltGPUInterface_Submit       (void)0
#else
	#define JoltGPUInterface_Shutdown     JoltGPUInterface_Shutdown
	#define JoltGPUInterface_Sync         JoltGPUInterface_Sync
	#define JoltGPUInterface_Submit       JoltGPUInterface_Submit
#endif

// 占位结构体，真实编译时由 Jolt 头文件提供
struct FZJoltBodyAlias {};
struct FZJoltConstraintAlias {};
struct FZJoltPhysicsSystemAlias {};
struct FZJoltBodyInterfaceAlias {};

namespace JoltAdapter
{
	inline int32 GetVersionMajor() { return 6; }
	inline int32 GetVersionMinor() { return 0; }
	inline int32 GetVersionPatch() { return 1; }
	inline FString GetVersionString() { return TEXT("6.0.1"); }
}
