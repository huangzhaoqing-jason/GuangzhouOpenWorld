#pragma once

// RHICrossPlatform.h
// Mac-only 渲染接口别名映射层
// 项目已锁定为 macOS Apple Silicon + Metal4.3，不再保留 Windows/DirectX 分支

#include "CoreMinimal.h"

#ifndef METAL_4_3
	#define METAL_4_3 1
#endif
#ifndef METAL_4
	#define METAL_4 1
#endif

// 接口别名：原 Windows 超前接口在 Mac 上映射到等效 MetalFX / FStreamableManager 实现
#define DLSS4_5_Enable                          MetalFX_Upscale_Enable
#define DLSS4_5_SetQuality                      MetalFX_Upscale_SetQuality
#define DLSS4_5_GetRecommendedSettings          MetalFX_Upscale_GetRecommendedSettings
#define FSR4_Enable                             MetalFX_Upscale_Enable
#define FSR4_SetQuality                         MetalFX_Upscale_SetQuality
#define FSR4_GetRecommendedSettings             MetalFX_Upscale_GetRecommendedSettings
#define DirectStorage_Initialize                FStreamableManager_Initialize
#define DirectStorage_EnqueueRequest            FStreamableManager_RequestAsyncLoad
#define DirectStorage_GetQueueCapacity          FStreamableManager_GetCapacity
#define DirectStorage_SetStagingBufferSize      FStreamableManager_SetMemoryLimit

#define METAL_4_3_FEATURE(X) X

// 统一类型别名，业务代码中可使用这些名称
using FMetal43RHIContext = void*;
using FMetal43CommandBuffer = void*;
using FDLSS45Handle = void*;
using FFSR4Handle = void*;
using FDirectStorageQueue = void*;

// 占位内联函数：实际引擎编译时由真实 RHI 提供
inline void MetalFX_Upscale_Enable(bool bEnable) {}
inline void MetalFX_Upscale_SetQuality(int32 Quality) {}
inline int32 MetalFX_Upscale_GetRecommendedSettings(int32 Width, int32 Height) { return 100; }
inline void FStreamableManager_Initialize() {}
inline void FStreamableManager_RequestAsyncLoad(const FString& Path) {}
inline int32 FStreamableManager_GetCapacity() { return 64; }
inline void FStreamableManager_SetMemoryLimit(int32 MB) {}
