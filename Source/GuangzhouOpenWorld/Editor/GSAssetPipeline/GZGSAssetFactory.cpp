#include "Editor/GSAssetPipeline/GZGSAssetFactory.h"

#if WITH_EDITOR
#include "Scene/GSRendering/GZGSAsset.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

UGZGSAssetFactory::UGZGSAssetFactory()
{
	SupportedClass = UGZGSAsset::StaticClass();
	Formats.Add(TEXT("ply;Gaussian Splatting PLY"));
	bCreateNew = false;
	bEditorImport = true;
	bText = false;
}

bool UGZGSAssetFactory::FactoryCanImport(const FString& Filename)
{
	return FPaths::GetExtension(Filename).Equals(TEXT("ply"), ESearchCase::IgnoreCase);
}

UObject* UGZGSAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	bOutOperationCanceled = false;

	UGZGSAsset* Asset = NewObject<UGZGSAsset>(InParent, InClass, InName, Flags);
	Asset->SourceFilePath = Filename;

	int32 VertexCount = 0;
	FBox ParsedBounds = FBox(FVector(-500.0f), FVector(500.0f));
	ParsePLYHeader(Filename, VertexCount, ParsedBounds);

	Asset->SplatCount = VertexCount;
	Asset->LocalBounds = ParsedBounds;

	// 根据顶点密度估算包围盒与碰撞体，避免只读点云
	const FVector Extent = ParsedBounds.GetExtent();
	Asset->CollisionExtent = Extent * 0.95f;
	Asset->TriggerExtent = Extent * 1.05f;
	Asset->bAffectsNavigation = true;

	return Asset;
}

bool UGZGSAssetFactory::ParsePLYHeader(const FString& FilePath, int32& OutVertexCount, FBox& OutBounds)
{
	OutVertexCount = 0;

	TArray<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
	{
		return false;
	}

	// 只读取头部 ASCII 部分（PLY 头部为 ASCII）
	FString Header;
	const int32 HeaderLen = FMath::Min(FileData.Num(), 4096);
	for (int32 i = 0; i < HeaderLen; ++i)
	{
		Header.AppendChar(static_cast<TCHAR>(FileData[i]));
	}

	// 解析 element vertex 数量
	const FString Token = TEXT("element vertex ");
	int32 Index = Header.Find(Token, ESearchCase::IgnoreCase);
	if (Index != INDEX_NONE)
	{
		const FString NumberPart = Header.Mid(Index + Token.Len(), 32).TrimStart();
		OutVertexCount = FCString::Atoi(*NumberPart);
	}

	// 无顶点位置信息时无法解析bounds，使用默认
	if (OutVertexCount <= 0)
	{
		OutBounds = FBox(FVector(-500.0f), FVector(500.0f));
		return false;
	}

	// 根据顶点数与经验密度估算默认边界（后续可在导入对话框中精修）
	const float EstimatedRadius = FMath::Pow(static_cast<float>(OutVertexCount), 1.0f / 3.0f) * 10.0f;
	OutBounds = FBox(FVector(-EstimatedRadius), FVector(EstimatedRadius));
	return true;
}

#endif
