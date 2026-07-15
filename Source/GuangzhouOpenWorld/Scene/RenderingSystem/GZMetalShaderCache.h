#pragma once

// GZMetalShaderCache.h
// WorldPartition 分区块 Metal Shader Library 缓存管理
// 运行期按玩家所在 cell 加载预编译 metallib，避免首次进入场景 JIT 卡顿

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/GZGameMode.h"
#include "GZMetalShaderCache.generated.h"

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZMetalShaderCache : public UObject
{
	GENERATED_BODY()

public:
	UGZMetalShaderCache();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	bool LoadLibraryForDistrict(EGZCityDistrict District);

	UFUNCTION(BlueprintCallable)
	bool UnloadLibraryForDistrict(EGZCityDistrict District);

	UFUNCTION(BlueprintCallable)
	void UnloadAllLibraries();

	UFUNCTION(BlueprintPure)
	bool IsLibraryLoaded(EGZCityDistrict District) const;

	UFUNCTION(BlueprintPure)
	int32 GetLoadedLibraryCount() const { return LoadedDistricts.Num(); }

private:
	FString GetLibraryPath(EGZCityDistrict District) const;
	FString DistrictToCellName(EGZCityDistrict District) const;

	UPROPERTY()
	TArray<EGZCityDistrict> LoadedDistricts;

	UPROPERTY()
	int32 MaxCachedLibraries = 4;
};
