#include "GZSoLoudResourceManager.h"
#include "Misc/Paths.h"

UGZSoLoudResourceManager::UGZSoLoudResourceManager()
{
}

void UGZSoLoudResourceManager::SetBasePath(const FString& BasePath)
{
	BaseResourcePath = BasePath;
}

void UGZSoLoudResourceManager::RegisterEvent(const FString& EventKey, const FString& RelativePath, EGZSoLoudLoadPriority Priority)
{
	FZSoLoudResourceEntry Entry;
	Entry.EventKey = EventKey;
	Entry.RelativePath = RelativePath;
	Entry.Priority = Priority;
	Entries.Add(Entry);
}

FString UGZSoLoudResourceManager::ResolvePath(const FString& EventKey) const
{
	for (const FZSoLoudResourceEntry& Entry : Entries)
	{
		if (Entry.EventKey == EventKey)
		{
			return FPaths::Combine(BaseResourcePath, Entry.RelativePath);
		}
	}
	return FString();
}

void UGZSoLoudResourceManager::RequestPreload(EGZSoLoudLoadPriority MinPriority)
{
	for (FZSoLoudResourceEntry& Entry : Entries)
	{
		if (Entry.Priority <= MinPriority && !Entry.bLoaded)
		{
			Entry.bLoaded = true;
			UE_LOG(LogTemp, Verbose, TEXT("[GZSoLoudResourceManager] Preloaded %s"), *Entry.EventKey);
		}
	}
}

void UGZSoLoudResourceManager::UnloadUnusedLowPriority()
{
	for (FZSoLoudResourceEntry& Entry : Entries)
	{
		if (Entry.Priority == EGZSoLoudLoadPriority::Low && Entry.bLoaded && !Entry.bUsedThisFrame)
		{
			Entry.bLoaded = false;
		}
		Entry.bUsedThisFrame = false;
	}
}

void UGZSoLoudResourceManager::MarkUsed(const FString& EventKey)
{
	for (FZSoLoudResourceEntry& Entry : Entries)
	{
		if (Entry.EventKey == EventKey)
		{
			Entry.bUsedThisFrame = true;
			break;
		}
	}
}
