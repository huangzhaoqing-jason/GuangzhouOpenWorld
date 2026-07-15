#pragma once

// GZGCDDispatch.h
// macOS Grand Central Dispatch 四级队列包装层
// 主队列 / 高优并行 / 默认并行 / 低优后台

#include "CoreMinimal.h"

namespace GZGCD
{
	UENUM(BlueprintType)
	enum class EDispatchQueue : uint8
	{
		Main            UMETA(DisplayName = "Main Queue"),
		HighPriority    UMETA(DisplayName = "High Priority Concurrent"),
		Default         UMETA(DisplayName = "Default Concurrent"),
		Background      UMETA(DisplayName = "Background Queue"),
	};

	// 异步投递任务到指定队列
	void Async(EDispatchQueue Queue, TFunction<void()> Task);

	// 同步等待任务在指定队列执行（主队列谨慎使用，避免死锁）
	void Sync(EDispatchQueue Queue, TFunction<void()> Task);

	// 并行 for：Body(index) 会在多个线程并行执行
	void ParallelFor(int32 Num, TFunction<void(int32 Index)> Body);
}
