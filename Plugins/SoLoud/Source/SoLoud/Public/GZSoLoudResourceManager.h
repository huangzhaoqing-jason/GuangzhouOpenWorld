#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZSoLoudResourceManager.generated.h"

UENUM(BlueprintType)
enum class EGZSoLoudLoadPriority : uint8
{
	Critical UMETA(DisplayName="Critical"),
	High     UMETA(DisplayName="High"),
	Normal   UMETA(DisplayName="Normal"),
	Low      UMETA(DisplayName="Low"),
};

USTRUCT(BlueprintType)
struct FZSoLoudResourceEntry
{
	GENERATED_BODY()

	UPROPERTY() FString EventKey;
	UPROPERTY() FString RelativePath;
	UPROPERTY() EGZSoLoudLoadPriority Priority = EGZSoLoudLoadPriority::Normal;
	UPROPERTY() bool bLoaded = false;
	UPROPERTY() bool bUsedThisFrame = false;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOLOUD_API UGZSoLoudResourceManager : public UObject
{
	GENERATED_BODY()

public:
	UGZSoLoudResourceManager();

	UFUNCTION(BlueprintCallable, Category="SoLoudResources")
	void SetBasePath(const FString& BasePath);

	UFUNCTION(BlueprintCallable, Category="SoLoudResources")
	void RegisterEvent(const FString& EventKey, const FString& RelativePath, EGZSoLoudLoadPriority Priority);

	UFUNCTION(BlueprintCallable, Category="SoLoudResources")
	FString ResolvePath(const FString& EventKey) const;

	UFUNCTION(BlueprintCallable, Category="SoLoudResources")
	void RequestPreload(EGZSoLoudLoadPriority MinPriority);

	UFUNCTION(BlueprintCallable, Category="SoLoudResources")
	void UnloadUnusedLowPriority();

	UFUNCTION(BlueprintCallable, Category="SoLoudResources")
	void MarkUsed(const FString& EventKey);

private:
	FString BaseResourcePath;
	TArray<FZSoLoudResourceEntry> Entries;
};
