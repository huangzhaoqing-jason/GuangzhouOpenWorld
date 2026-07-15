#pragma once

#include "CoreMinimal.h"

/**
 * SwiftUI APP 与 UE5 之间的 Metal 共享纹理描述。
 * 真实实现需在 macOS 上通过 IOSurface / MTLSharedTexture 创建跨进程纹理。
 * Linux Headless 下仅保留空结构，不参与编译链接。
 */
USTRUCT(BlueprintType)
struct FMetalSharedTextureDesc
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 Width = 1920;

	UPROPERTY(BlueprintReadWrite)
	int32 Height = 1080;

	UPROPERTY(BlueprintReadWrite)
	FString PixelFormat = TEXT("BGRA8Unorm");

	UPROPERTY(BlueprintReadWrite)
	uint64 SharedTextureHandle = 0;

	UPROPERTY(BlueprintReadWrite)
	bool bIsValid = false;

#if PLATFORM_MAC
	/**
	 * 申请/更新共享纹理。实际实现将创建 IOSurface 并通过 Mach port 返回句柄。
	 * 当前版本保留接口，供后续 Spark 2.0 ↔ UE5 桥接实现。
	 */
	void AllocateOnMac();
#endif
};
