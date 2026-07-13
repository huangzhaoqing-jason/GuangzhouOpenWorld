#include "Scene/RenderingSystem/GZPCGGenerator.h"
#include "Scene/RenderingSystem/GZWorldPartition.h"
#include "GuangzhouOpenWorld.h"
#include "Math/UnrealMathUtility.h"

int32 UGZPCGGenerator::GetTemplateCountForDistrict(EGZDistrictType Type)
{
	switch (Type)
	{
	case EGZDistrictType::Qilou: return QilouTemplateCount;
	case EGZDistrictType::CBD: return CBDTemplateCount;
	case EGZDistrictType::Residential: return ResidentialTemplateCount;
	case EGZDistrictType::OldTown: return QilouTemplateCount;
	case EGZDistrictType::Industrial: return 24;
	case EGZDistrictType::Park: return 8;
	case EGZDistrictType::Riverfront: return 16;
	case EGZDistrictType::University: return 20;
	default: return 16;
	}
}

UGZPCGGenerator::UGZPCGGenerator()
{
}

void UGZPCGGenerator::Initialize(UGZWorldPartition* InWorldPartition)
{
	WorldPartition = InWorldPartition;
	GeneratedTemplates.Empty();
	InitializeDistricts();
	bIsInitialized = true;

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("PCG Generator: %d districts, %d qilou, %d CBD, %d residential, %.0f%% interiors, seed=%d, clutter: old town=%.0f%%, CBD=%.0f%%, efficiency core=%d"),
		DistrictCount, QilouTemplateCount, CBDTemplateCount, ResidentialTemplateCount,
		DefaultInteriorProbability * 100.0f, GlobalSeed, OldTownClutterDensity * 100.0f, CBDClutterDensity * 100.0f, EfficiencyCore);
}

void UGZPCGGenerator::GenerateAll()
{
	if (!bIsInitialized) return;

	for (const FGZDistrictDefinition& District : Districts)
	{
		GenerateDistrict(District);
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("PCG: generated %d templates across %d districts"), GeneratedTemplates.Num(), Districts.Num());
}

void UGZPCGGenerator::GenerateDistrict(const FGZDistrictDefinition& District)
{
	TArray<FGZPCGTemplate> DistrictTemplates;
	GenerateTemplatesForDistrict(District, DistrictTemplates);
	GeneratedTemplates.Append(DistrictTemplates);
	AddClutter(District);
}

void UGZPCGGenerator::GenerateTemplatesForDistrict(const FGZDistrictDefinition& District, TArray<FGZPCGTemplate>& OutTemplates)
{
	OutTemplates.Empty();
	int32 TemplateCount = District.TemplateCount;
	if (TemplateCount <= 0) TemplateCount = GetTemplateCountForDistrict(District.Type);

	OutTemplates.Reserve(TemplateCount);

	float Width = District.BoundsMax.X - District.BoundsMin.X;
	float Height = District.BoundsMax.Y - District.BoundsMin.Y;
	float AreaPerTemplate = (Width * Height) / TemplateCount;
	float CellSpacing = FMath::Sqrt(AreaPerTemplate);

	int32 GridX = FMath::Max(1, FMath::RoundToInt(Width / CellSpacing));
	int32 GridY = FMath::Max(1, FMath::RoundToInt(Height / CellSpacing));

	int32 TemplateIndex = 0;
	for (int32 Y = 0; Y < GridY && TemplateIndex < TemplateCount; ++Y)
	{
		for (int32 X = 0; X < GridX && TemplateIndex < TemplateCount; ++X)
		{
			int32 CombinedSeed = GetCombinedSeed(District.Seed, 0, TemplateIndex);
			FMath::SRandInit(CombinedSeed);

			FGZPCGTemplate Tpl;
			Tpl.TemplateID = TemplateIndex;
			Tpl.DistrictType = District.Type;

			float JitterX = FMath::FRandRange(-0.3f, 0.3f) * CellSpacing;
			float JitterY = FMath::FRandRange(-0.3f, 0.3f) * CellSpacing;
			float PosX = District.BoundsMin.X + (X + 0.5f) * CellSpacing + JitterX;
			float PosY = District.BoundsMin.Y + (Y + 0.5f) * CellSpacing + JitterY;

			Tpl.Position = FVector(PosX, PosY, 0.0f);
			Tpl.Rotation = FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f);
			Tpl.Scale = FVector(FMath::FRandRange(0.8f, 1.2f));

			FMath::SRandInit(CombinedSeed + 1000);
			Tpl.bHasInterior = FMath::FRand() < District.InteriorProbability;
			Tpl.bIsGenerated = true;

			OutTemplates.Add(Tpl);
			TemplateIndex++;
		}
	}
}

void UGZPCGGenerator::AddClutter(const FGZDistrictDefinition& District)
{
	float ClutterDensity = GetClutterDensityForDistrict(District.Type);
	int32 ClutterCount = FMath::RoundToInt(District.TemplateCount * ClutterDensity);

	FMath::SRandInit(District.Seed + 9999);

	for (int32 i = 0; i < ClutterCount; ++i)
	{
		float Width = District.BoundsMax.X - District.BoundsMin.X;
		float Height = District.BoundsMax.Y - District.BoundsMin.Y;
		FVector ClutterPos(
			District.BoundsMin.X + FMath::FRand() * Width,
			District.BoundsMin.Y + FMath::FRand() * Height,
			0.0f
		);
	}
}

void UGZPCGGenerator::SetSeed(int32 NewSeed)
{
	GlobalSeed = NewSeed;
	GeneratedTemplates.Empty();
	InitializeDistricts();
}

void UGZPCGGenerator::InitializeDistricts()
{
	Districts.Empty();
	Districts.Reserve(DistrictCount);

	FGZDistrictDefinition D1;
	D1.Type = EGZDistrictType::Qilou;
	D1.Name = TEXT("Shangxiajiu Qilou");
	D1.BoundsMin = FVector2D(-5000.0f, -3000.0f);
	D1.BoundsMax = FVector2D(-2000.0f, 0.0f);
	D1.TemplateCount = QilouTemplateCount;
	D1.ClutterDensity = OldTownClutterDensity;
	D1.InteriorProbability = 0.9f;
	D1.Seed = GlobalSeed + 1;
	Districts.Add(D1);

	FGZDistrictDefinition D2;
	D2.Type = EGZDistrictType::CBD;
	D2.Name = TEXT("Zhujiang New Town CBD");
	D2.BoundsMin = FVector2D(2000.0f, 2000.0f);
	D2.BoundsMax = FVector2D(6000.0f, 6000.0f);
	D2.TemplateCount = CBDTemplateCount;
	D2.ClutterDensity = CBDClutterDensity;
	D2.InteriorProbability = 0.9f;
	D2.Seed = GlobalSeed + 2;
	Districts.Add(D2);

	FGZDistrictDefinition D3;
	D3.Type = EGZDistrictType::Residential;
	D3.Name = TEXT("Tianhe Residential");
	D3.BoundsMin = FVector2D(-1000.0f, 3000.0f);
	D3.BoundsMax = FVector2D(2000.0f, 6000.0f);
	D3.TemplateCount = ResidentialTemplateCount;
	D3.ClutterDensity = 0.5f;
	D3.InteriorProbability = 0.9f;
	D3.Seed = GlobalSeed + 3;
	Districts.Add(D3);

	FGZDistrictDefinition D4;
	D4.Type = EGZDistrictType::OldTown;
	D4.Name = TEXT("Liwan Old Town");
	D4.BoundsMin = FVector2D(-6000.0f, -6000.0f);
	D4.BoundsMax = FVector2D(-3000.0f, -3000.0f);
	D4.TemplateCount = QilouTemplateCount;
	D4.ClutterDensity = OldTownClutterDensity;
	D4.InteriorProbability = 0.9f;
	D4.Seed = GlobalSeed + 4;
	Districts.Add(D4);

	FGZDistrictDefinition D5;
	D5.Type = EGZDistrictType::Industrial;
	D5.Name = TEXT("Panyu Industrial");
	D5.BoundsMin = FVector2D(5000.0f, -4000.0f);
	D5.BoundsMax = FVector2D(8000.0f, -1000.0f);
	D5.TemplateCount = 24;
	D5.ClutterDensity = 0.4f;
	D5.InteriorProbability = 0.5f;
	D5.Seed = GlobalSeed + 5;
	Districts.Add(D5);

	FGZDistrictDefinition D6;
	D6.Type = EGZDistrictType::Park;
	D6.Name = TEXT("Baiyun Mountain Park");
	D6.BoundsMin = FVector2D(-3000.0f, 6000.0f);
	D6.BoundsMax = FVector2D(0.0f, 8000.0f);
	D6.TemplateCount = 8;
	D6.ClutterDensity = 0.2f;
	D6.InteriorProbability = 0.1f;
	D6.Seed = GlobalSeed + 6;
	Districts.Add(D6);

	FGZDistrictDefinition D7;
	D7.Type = EGZDistrictType::Riverfront;
	D7.Name = TEXT("Pearl Riverfront");
	D7.BoundsMin = FVector2D(-4000.0f, -1000.0f);
	D7.BoundsMax = FVector2D(4000.0f, 1000.0f);
	D7.TemplateCount = 16;
	D7.ClutterDensity = 0.3f;
	D7.InteriorProbability = 0.3f;
	D7.Seed = GlobalSeed + 7;
	Districts.Add(D7);

	FGZDistrictDefinition D8;
	D8.Type = EGZDistrictType::University;
	D8.Name = TEXT("Guangzhou University City");
	D8.BoundsMin = FVector2D(6000.0f, 4000.0f);
	D8.BoundsMax = FVector2D(9000.0f, 7000.0f);
	D8.TemplateCount = 20;
	D8.ClutterDensity = 0.4f;
	D8.InteriorProbability = 0.9f;
	D8.Seed = GlobalSeed + 8;
	Districts.Add(D8);
}

void UGZPCGGenerator::RegisterDistrictTemplates()
{
	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Registered %d district templates"), Districts.Num());
}

int32 UGZPCGGenerator::GetCombinedSeed(int32 BaseSeed, int32 DistrictIndex, int32 TemplateIndex) const
{
	return BaseSeed ^ (DistrictIndex * 7919) ^ (TemplateIndex * 6271);
}

float UGZPCGGenerator::GetClutterDensityForDistrict(EGZDistrictType Type) const
{
	switch (Type)
	{
	case EGZDistrictType::OldTown: return OldTownClutterDensity;
	case EGZDistrictType::CBD: return CBDClutterDensity;
	case EGZDistrictType::Qilou: return OldTownClutterDensity;
	case EGZDistrictType::Residential: return 0.5f;
	case EGZDistrictType::Industrial: return 0.4f;
	case EGZDistrictType::Park: return 0.2f;
	case EGZDistrictType::Riverfront: return 0.3f;
	case EGZDistrictType::University: return 0.4f;
	default: return 0.5f;
	}
}