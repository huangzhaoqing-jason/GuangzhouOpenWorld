#pragma once

// RHICrossPlatform.h
// 跨平台渲染接口别名映射层
// 原则：保留全部原有代码、参数、架构、命名，仅通过别名实现 Mac/Windows 兼容

#include "CoreMinimal.h"

#if PLATFORM_MAC
	// Mac 平台：Metal4.3 标识映射到 Metal4 实际 API，保留 Metal4.3 专属代码的条件编译
	#ifndef METAL_4_3
		#define METAL_4_3 1
	#endif
	#ifndef METAL_4
		#define METAL_4 1
	#endif

	// 接口别名：Windows 超前接口在 Mac 上映射到等效实现或占位宏
	#define DLSS4_5_Enable                          MetalFX_Upscale_Enable
	#define DLSS4_5_SetQuality                      MetalFX_Upscale_SetQuality
	#define DLSS4_5_GetRecommendedSettings          MetalFX_Upscale_GetRecommendedSettings
	#define FSR4_Enable                             MetalFX_Upscale_Enable
	#define FSR4_SetQuality                         MetalFX_Upscale_SetQuality
	#define FSR4_GetRecommendedSettings             MetalFX_Upscale_GetRecommendedSettings
	#define DirectStorage_Initialize                FStreamableManager_Initialize
	#define DirectStorage_EnqueueRequest           FStreamableManager_RequestAsyncLoad
	#define DirectStorage_GetQueueCapacity         FStreamableManager_GetCapacity
	#define DirectStorage_SetStagingBufferSize     FStreamableManager_SetMemoryLimit

	// Metal4.3 专属代码保留条件编译，不删除
	#if METAL_4_3
		#define METAL_4_3_FEATURE(X) X
	#else
		#define METAL_4_3_FEATURE(X)
	#endif
#else
	// Windows 平台：保留原版全部超前接口，不做任何映射
	#ifndef METAL_4_3
		#define METAL_4_3 0
	#endif
	#define DLSS4_5_Enable                          DLSS4_5_Enable
	#define DLSS4_5_SetQuality                      DLSS4_5_SetQuality
	#define DLSS4_5_GetRecommendedSettings          DLSS4_5_GetRecommendedSettings
	#define FSR4_Enable                             FSR4_Enable
	#define FSR4_SetQuality                         FSR4_SetQuality
	#define FSR4_GetRecommendedSettings             FSR4_GetRecommendedSettings
	#define DirectStorage_Initialize                DirectStorage_Initialize
	#define DirectStorage_EnqueueRequest           DirectStorage_EnqueueRequest
	#define DirectStorage_GetQueueCapacity         DirectStorage_GetQueueCapacity
	#define DirectStorage_SetStagingBufferSize     DirectStorage_SetStagingBufferSize

	#define METAL_4_3_FEATURE(X)
#endif

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
