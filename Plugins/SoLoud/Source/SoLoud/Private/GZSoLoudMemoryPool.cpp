#include "GZSoLoudMemoryPool.h"
#include "HAL/UnrealMemory.h"
#include "HAL/PlatformMemory.h"

FZSoLoudMemoryPool::FZSoLoudMemoryPool(int32 InBlockSize, int32 InBlockCount)
	: BlockSize(InBlockSize), BlockCount(InBlockCount), UsedBlocks(0)
{
	PoolBase = (uint8*)FMemory::Malloc(BlockSize * BlockCount, 64);
	FreeList.Reserve(BlockCount);
	for (int32 i = 0; i < BlockCount; ++i)
	{
		FreeList.Add(PoolBase + i * BlockSize);
	}
}

FZSoLoudMemoryPool::~FZSoLoudMemoryPool()
{
	FMemory::Free(PoolBase);
}

void* FZSoLoudMemoryPool::Allocate()
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

void FZSoLoudMemoryPool::Free(void* Ptr)
{
	if (!Ptr) return;
	FScopeLock Lock(&CriticalSection);
	FreeList.Add(Ptr);
	UsedBlocks--;
}

void FZSoLoudMemoryPool::Reset()
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

FZSoLoudMemoryPoolStats FZSoLoudMemoryPool::GetStats() const
{
	FScopeLock Lock(&CriticalSection);
	FZSoLoudMemoryPoolStats Stats;
	Stats.PoolCount = BlockCount;
	Stats.UsedBlocks = UsedBlocks;
	Stats.FreeBlocks = FreeList.Num();
	Stats.TotalBytes = BlockSize * BlockCount;
	return Stats;
}

FZSoLoudMemoryManager& FZSoLoudMemoryManager::Get()
{
	static FZSoLoudMemoryManager Instance;
	return Instance;
}

void FZSoLoudMemoryManager::Initialize()
{
	StreamPool = MakeUnique<FZSoLoudMemoryPool>(4096, 8192);
	SFXPool = MakeUnique<FZSoLoudMemoryPool>(256, 65536);
	GeneralPool = MakeUnique<FZSoLoudMemoryPool>(1024, 16384);
}

void FZSoLoudMemoryManager::Shutdown()
{
	StreamPool.Reset();
	SFXPool.Reset();
	GeneralPool.Reset();
	{
		FScopeLock Lock(&FallbackCriticalSection);
		FallbackAllocations.Reset();
	}
}

void* FZSoLoudMemoryManager::Allocate(size_t Size)
{
	if (Size <= 256)
	{
		void* Result = SFXPool->Allocate();
		if (Result) return Result;
	}
	else if (Size <= 1024)
	{
		void* Result = GeneralPool->Allocate();
		if (Result) return Result;
	}
	else if (Size <= 4096)
	{
		void* Result = StreamPool->Allocate();
		if (Result) return Result;
	}

	FScopeLock Lock(&FallbackCriticalSection);
	void* Fallback = FMemory::Malloc(Size, 64);
	FallbackAllocations.Add(Fallback, Size);
	return Fallback;
}

void FZSoLoudMemoryManager::Free(void* Ptr)
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
	if (SFXPool.IsValid()) SFXPool->Free(Ptr);
	else if (GeneralPool.IsValid()) GeneralPool->Free(Ptr);
	else if (StreamPool.IsValid()) StreamPool->Free(Ptr);
}

FZSoLoudMemoryPoolStats FZSoLoudMemoryManager::GetStreamPoolStats() const
{
	return StreamPool.IsValid() ? StreamPool->GetStats() : FZSoLoudMemoryPoolStats();
}

FZSoLoudMemoryPoolStats FZSoLoudMemoryManager::GetSFXPoolStats() const
{
	return SFXPool.IsValid() ? SFXPool->GetStats() : FZSoLoudMemoryPoolStats();
}

FZSoLoudMemoryPoolStats FZSoLoudMemoryManager::GetGeneralPoolStats() const
{
	return GeneralPool.IsValid() ? GeneralPool->GetStats() : FZSoLoudMemoryPoolStats();
}
