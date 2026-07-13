#pragma once

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#include "GZSoLoudMemoryPool.generated.h"

USTRUCT(BlueprintType)
struct FZSoLoudMemoryPoolStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 PoolCount = 0;
	UPROPERTY(BlueprintReadOnly) int32 UsedBlocks = 0;
	UPROPERTY(BlueprintReadOnly) int32 FreeBlocks = 0;
	UPROPERTY(BlueprintReadOnly) int32 TotalBytes = 0;
};

class FZSoLoudMemoryPool
{
public:
	FZSoLoudMemoryPool(int32 InBlockSize, int32 InBlockCount);
	~FZSoLoudMemoryPool();

	void* Allocate();
	void Free(void* Ptr);
	void Reset();
	FZSoLoudMemoryPoolStats GetStats() const;

private:
	int32 BlockSize = 0;
	int32 BlockCount = 0;
	int32 UsedBlocks = 0;
	uint8* PoolBase = nullptr;
	TArray<void*> FreeList;
	mutable FCriticalSection CriticalSection;
};

class FZSoLoudMemoryManager
{
public:
	static FZSoLoudMemoryManager& Get();

	void Initialize();
	void Shutdown();

	void* Allocate(size_t Size);
	void Free(void* Ptr);

	FZSoLoudMemoryPoolStats GetStreamPoolStats() const;
	FZSoLoudMemoryPoolStats GetSFXPoolStats() const;
	FZSoLoudMemoryPoolStats GetGeneralPoolStats() const;

private:
	FZSoLoudMemoryManager() = default;
	FZSoLoudMemoryManager(const FZSoLoudMemoryManager&) = delete;

	TUniquePtr<FZSoLoudMemoryPool> StreamPool;
	TUniquePtr<FZSoLoudMemoryPool> SFXPool;
	TUniquePtr<FZSoLoudMemoryPool> GeneralPool;

	FCriticalSection FallbackCriticalSection;
	TMap<void*, size_t> FallbackAllocations;
};
