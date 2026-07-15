#pragma once

// JoltVersionAdapter.h
// Jolt Physics v6.0.1 版本适配层（macOS Apple Silicon 专用）
// 项目已彻底剔除 Windows 分支，GPU 加速接口直接屏蔽

#include "CoreMinimal.h"

#ifndef JOLT_VERSION_601
	#define JOLT_VERSION_601 1
#endif

// 保留 Jolt 核心类名/函数名别名，上层调用代码不变
#define JoltBody              FZJoltBodyAlias
#define JoltConstraint        FZJoltConstraintAlias
#define JoltPhysicsSystem     FZJoltPhysicsSystemAlias
#define JoltBodyInterface     FZJoltBodyInterfaceAlias

// Mac 平台屏蔽 v6 专属 GPU 加速接口；Windows 分支已移除
#define JoltGPUInterface_Shutdown     (void)0
#define JoltGPUInterface_Sync         (void)0
#define JoltGPUInterface_Submit       (void)0

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
