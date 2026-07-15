#include "Scene/RenderingSystem/GZMetalShaderCache.h"
#include "GuangzhouOpenWorld.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

UGZMetalShaderCache::UGZMetalShaderCache()
{
}

void UGZMetalShaderCache::Initialize()
{
	LoadedDistricts.Empty();
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Metal Shader Cache initialized. Cache root: %s"), *FPaths::ProjectContentDir());
}

bool UGZMetalShaderCache::LoadLibraryForDistrict(EGZCityDistrict District)
{
	if (IsLibraryLoaded(District))
	{
		return true;
	}

	FString LibraryPath = GetLibraryPath(District);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (!PlatformFile.FileExists(*LibraryPath))
	{
		UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("Metal library not found for district %d: %s"), (int32)District, *LibraryPath);
		return false;
	}

	// 简单 LRU：超过缓存上限时卸载最早加载的
	while (LoadedDistricts.Num() >= MaxCachedLibraries)
	{
		UnloadLibraryForDistrict(LoadedDistricts[0]);
	}

	// 占位：真实实现需调用 Metal API 加载 metallib
	LoadedDistricts.Add(District);
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Metal library loaded for district %d"), (int32)District);
	return true;
}

bool UGZMetalShaderCache::UnloadLibraryForDistrict(EGZCityDistrict District)
{
	int32 Index = LoadedDistricts.Find(District);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	// 占位：真实实现需释放 Metal MTLLibrary 对象
	LoadedDistricts.RemoveAt(Index);
	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Metal library unloaded for district %d"), (int32)District);
	return true;
}

void UGZMetalShaderCache::UnloadAllLibraries()
{
	LoadedDistricts.Empty();
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("All Metal shader libraries unloaded"));
}

bool UGZMetalShaderCache::IsLibraryLoaded(EGZCityDistrict District) const
{
	return LoadedDistricts.Contains(District);
}

FString UGZMetalShaderCache::GetLibraryPath(EGZCityDistrict District) const
{
	return FPaths::ProjectContentDir() / TEXT("MetalShaderLibraries") / (DistrictToCellName(District) + TEXT(".metallib"));
}

FString UGZMetalShaderCache::DistrictToCellName(EGZCityDistrict District) const
{
	switch (District)
	{
	case EGZCityDistrict::TianheCBD:        return TEXT("CBD");
	case EGZCityDistrict::YuexiuOldTown:    return TEXT("OldTown");
	case EGZCityDistrict::LiwanXiguan:      return TEXT("Xiguan");
	case EGZCityDistrict::HaizhuInnovation: return TEXT("Innovation");
	case EGZCityDistrict::BaiyunSuburban:   return TEXT("Suburban");
	case EGZCityDistrict::PanyuUniversity:  return TEXT("University");
	case EGZCityDistrict::HuangpuPort:      return TEXT("Port");
	case EGZCityDistrict::NanshaNewArea:    return TEXT("Nansha");
	case EGZCityDistrict::PearlRiver:       return TEXT("River");
	default:                                return TEXT("Common");
	}
}
