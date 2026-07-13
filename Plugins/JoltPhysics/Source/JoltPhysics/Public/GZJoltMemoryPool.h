#pragma once

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#include "Templates/Function.h"
#include "GZJoltMemoryPool.generated.h"

USTRUCT(BlueprintType)
struct FZJoltMemoryPoolStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 PoolCount = 0;
	UPROPERTY(BlueprintReadOnly) int32 UsedBlocks = 0;
	UPROPERTY(BlueprintReadOnly) int32 FreeBlocks = 0;
	UPROPERTY(BlueprintReadOnly) int32 TotalBytes = 0;
};

class FZJoltMemoryPool
{
public:
	FZJoltMemoryPool(int32 InBlockSize, int32 InBlockCount);
	~FZJoltMemoryPool();

	void* Allocate();
	void Free(void* Ptr);
	void Reset();
	FZJoltMemoryPoolStats GetStats() const;

private:
	int32 BlockSize = 0;
	int32 BlockCount = 0;
	int32 UsedBlocks = 0;
	uint8* PoolBase = nullptr;
	TArray<void*> FreeList;
	mutable FCriticalSection CriticalSection;
};

class FZJoltMemoryManager
{
public:
	static FZJoltMemoryManager& Get();

	void Initialize();
	void Shutdown();

	void* Allocate(size_t Size);
	void Free(void* Ptr);

	FZJoltMemoryPoolStats GetAIPoolStats() const;
	FZJoltMemoryPoolStats GetVehiclePoolStats() const;
	FZJoltMemoryPoolStats GetGeneralPoolStats() const;

	void SetMemoryLimitMB(int32 LimitMB);
	int32 GetMemoryLimitMB() const;

private:
	FZJoltMemoryManager() = default;
	FZJoltMemoryManager(const FZJoltMemoryManager&) = delete;

	TUniquePtr<FZJoltMemoryPool> AIPool;
	TUniquePtr<FZJoltMemoryPool> VehiclePool;
	TUniquePtr<FZJoltMemoryPool> GeneralPool;

	int32 MemoryLimitMB = 2048;
	FCriticalSection FallbackCriticalSection;
	TMap<void*, size_t> FallbackAllocations;
};
