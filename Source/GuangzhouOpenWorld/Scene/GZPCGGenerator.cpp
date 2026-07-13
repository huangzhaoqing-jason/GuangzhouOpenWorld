#include "GZPCGGenerator.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AGZPCGGenerator::AGZPCGGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
    RNG.Initialize(BuildingSeed);
    // Default building templates
    BuildingTemplates = {
        {FVector(8, 25, 8), FLinearColor(0.55, 0.55, 0.65), 0.3f, false, false},
        {FVector(12, 60, 12), FLinearColor(0.5, 0.55, 0.7), 0.5f, true, true},
        {FVector(6, 15, 6), FLinearColor(0.7, 0.65, 0.55), 0.1f, false, false},
        {FVector(10, 8, 10), FLinearColor(0.6, 0.6, 0.6), 0.2f, false, true},
        {FVector(15, 95, 15), FLinearColor(0.45, 0.5, 0.65), 0.7f, true, true},
        {FVector(20, 120, 20), FLinearColor(0.4, 0.45, 0.6), 0.8f, true, true},
    };
}

void AGZPCGGenerator::BeginPlay()
{
    Super::BeginPlay();
    GenerateCity();
}

void AGZPCGGenerator::GenerateCity()
{
    UE_LOG(LogTemp, Log, TEXT("PCG: Generating Guangzhou City..."));
    // Tianhe CBD
    GenerateDistrict(TEXT("Tianhe CBD"), FVector(CBDCenterX, 0, CBDCenterY), CBDRadius, 300);
    // Yuexiu Old Town
    GenerateDistrict(TEXT("Yuexiu"), FVector(-300, 0, -200), 200, 150);
    // Liwan
    GenerateDistrict(TEXT("Liwan"), FVector(-400, 0, -100), 180, 120);
    // Haizhu
    GenerateDistrict(TEXT("Haizhu"), FVector(0, 0, 300), 220, 180);
    // Baiyun Mountain
    GenerateDistrict(TEXT("Baiyun"), FVector(100, 0, -400), 250, 80);
    // Pazhou
    GenerateDistrict(TEXT("Pazhou"), FVector(300, 0, 200), 150, 100);
    // Nansha
    GenerateDistrict(TEXT("Nansha"), FVector(400, 0, -400), 150, 90);
    // University City
    GenerateDistrict(TEXT("University"), FVector(200, 0, 350), 120, 70);
    // Road network
    GenerateRoadNetwork();
    // Parks and trees
    GenerateParksAndTrees();
    UE_LOG(LogTemp, Log, TEXT("PCG: City generation complete!"));
}

void AGZPCGGenerator::GenerateDistrict(FString DistrictName, FVector Center, float Radius, int32 BuildingCount)
{
    UE_LOG(LogTemp, Log, TEXT("PCG: Generating %s district (%d buildings)"), *DistrictName, BuildingCount);
    bool bIsCBD = DistrictName.Contains(TEXT("CBD"));
    for (int32 i = 0; i < BuildingCount; i++)
    {
        FVector Location = GetPCGPosition(Center, Radius);
        int32 TemplateIdx = bIsCBD ? RNG.RandRange(1, BuildingTemplates.Num() - 1) : RNG.RandRange(0, 2);
        SpawnBuilding(Location, BuildingTemplates[TemplateIdx]);
    }
}

void AGZPCGGenerator::GenerateRoadNetwork()
{
    UE_LOG(LogTemp, Log, TEXT("PCG: Generating road network..."));
    // Major roads every 80m
    for (int32 i = -MapSize / 2; i <= MapSize / 2; i += 80)
    {
        // Horizontal road
        FVector HLoc(i, 0, 0);
        // Vertical road
        FVector VLoc(0, 0, i);
    }
    // Pearl River bridge crossings
    // Bridge at z=80 (main crossing)
    // Additional bridges at z=-200, z=200
}

void AGZPCGGenerator::GenerateParksAndTrees()
{
    // Generate trees throughout the city
    for (int32 i = 0; i < 5000; i++)
    {
        FVector Loc(FMath::RandRange(-4000, 4000), 0, FMath::RandRange(-4000, 4000));
        // Skip if on road (simplified check)
        bool bOnRoad = (FMath::Fmod(FMath::Abs(Loc.X), 80) < 3 || FMath::Fmod(FMath::Abs(Loc.Z), 80) < 3);
        if (!bOnRoad && FMath::FRand() < 0.15f)
        {
            // Spawn tree at Loc
        }
    }
}

void AGZPCGGenerator::SpawnBuilding(FVector Location, const FBuildingTemplate& Template)
{
    // Spawn a static mesh actor as a building
    // In full implementation, this would use HISM or Nanite-enabled meshes
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AActor* Building = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), Location, FRotator::ZeroRotator, Params);
    if (Building)
    {
        UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(Building);
        Mesh->RegisterComponent();
        Building->SetRootComponent(Mesh);
        // Set scale based on template
        Building->SetActorScale3D(Template.Size / 100.0f);
    }
}

FVector AGZPCGGenerator::GetPCGPosition(FVector Base, float Variance) const
{
    float Angle = FMath::FRand() * PI * 2.0f;
    float Dist = FMath::Sqrt(FMath::FRand()) * Variance;
    return Base + FVector(FMath::Cos(Angle) * Dist, 0, FMath::Sin(Angle) * Dist);
}