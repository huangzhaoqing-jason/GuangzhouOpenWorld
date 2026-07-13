#include "GZJoltMemoryPool.h"
#include "HAL/UnrealMemory.h"
#include "HAL/PlatformMemory.h"

FZJoltMemoryPool::FZJoltMemoryPool(int32 InBlockSize, int32 InBlockCount)
	: BlockSize(InBlockSize), BlockCount(InBlockCount), UsedBlocks(0)
{
	PoolBase = (uint8*)FMemory::Malloc(BlockSize * BlockCount, 64);
	FreeList.Reserve(BlockCount);
	for (int32 i = 0; i < BlockCount; ++i)
	{
		FreeList.Add(PoolBase + i * BlockSize);
	}
}

FZJoltMemoryPool::~FZJoltMemoryPool()
{
	FMemory::Free(PoolBase);
}

void* FZJoltMemoryPool::Allocate()
{
	FScopeLock Lock(&CriticalSection);
	if (FreeList.Num() == 0)
	{
		return nullptr;
	}
	void* Result = FreeList.Pop();
	UsedBlocks++;
	return Result;
}

void FZJoltMemoryPool::Free(void* Ptr)
{
	if (!Ptr) return;
	FScopeLock Lock(&CriticalSection);
	FreeList.Add(Ptr);
	UsedBlocks--;
}

void FZJoltMemoryPool::Reset()
{
	FScopeLock Lock(&CriticalSection);
	FreeList.Reset();
	FreeList.Reserve(BlockCount);
	for (int32 i = 0; i < BlockCount; ++i)
	{
		FreeList.Add(PoolBase + i * BlockSize);
	}
	UsedBlocks = 0;
}

FZJoltMemoryPoolStats FZJoltMemoryPool::GetStats() const
{
	FScopeLock Lock(&CriticalSection);
	FZJoltMemoryPoolStats Stats;
	Stats.PoolCount = BlockCount;
	Stats.UsedBlocks = UsedBlocks;
	Stats.FreeBlocks = FreeList.Num();
	Stats.TotalBytes = BlockSize * BlockCount;
	return Stats;
}

FZJoltMemoryManager& FZJoltMemoryManager::Get()
{
	static FZJoltMemoryManager Instance;
	return Instance;
}

void FZJoltMemoryManager::Initialize()
{
	AIPool = MakeUnique<FZJoltMemoryPool>(256, 65536);
	VehiclePool = MakeUnique<FZJoltMemoryPool>(1024, 8192);
	GeneralPool = MakeUnique<FZJoltMemoryPool>(512, 16384);
}

void FZJoltMemoryManager::Shutdown()
{
	AIPool.Reset();
	VehiclePool.Reset();
	GeneralPool.Reset();
	{
		FScopeLock Lock(&FallbackCriticalSection);
		FallbackAllocations.Reset();
	}
}

void* FZJoltMemoryManager::Allocate(size_t Size)
{
	if (Size <= 256)
	{
		void* Result = AIPool->Allocate();
		if (Result) return Result;
	}
	else if (Size <= 1024)
	{
		void* Result = VehiclePool->Allocate();
		if (Result) return Result;
	}

	void* Result = GeneralPool->Allocate();
	if (Result) return Result;

	FScopeLock Lock(&FallbackCriticalSection);
	void* Fallback = FMemory::Malloc(Size, 64);
	FallbackAllocations.Add(Fallback, Size);
	return Fallback;
}

void FZJoltMemoryManager::Free(void* Ptr)
{
	if (!Ptr) return;
	{
		FScopeLock Lock(&FallbackCriticalSection);
		if (FallbackAllocations.Contains(Ptr))
		{
			FMemory::Free(Ptr);
			FallbackAllocations.Remove(Ptr);
			return;
		}
	}
	if (AIPool.IsValid()) AIPool->Free(Ptr);
	else if (VehiclePool.IsValid()) VehiclePool->Free(Ptr);
	else if (GeneralPool.IsValid()) GeneralPool->Free(Ptr);
}

FZJoltMemoryPoolStats FZJoltMemoryManager::GetAIPoolStats() const
{
	return AIPool.IsValid() ? AIPool->GetStats() : FZJoltMemoryPoolStats();
}

FZJoltMemoryPoolStats FZJoltMemoryManager::GetVehiclePoolStats() const
{
	return VehiclePool.IsValid() ? VehiclePool->GetStats() : FZJoltMemoryPoolStats();
}

FZJoltMemoryPoolStats FZJoltMemoryManager::GetGeneralPoolStats() const
{
	return GeneralPool.IsValid() ? GeneralPool->GetStats() : FZJoltMemoryPoolStats();
}

void FZJoltMemoryManager::SetMemoryLimitMB(int32 LimitMB)
{
	MemoryLimitMB = LimitMB;
}

int32 FZJoltMemoryManager::GetMemoryLimitMB() const
{
	return MemoryLimitMB;
}
