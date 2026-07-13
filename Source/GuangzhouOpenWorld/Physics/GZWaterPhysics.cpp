#include "GZWaterPhysics.h"
#include "ProceduralMeshComponent.h"

AGZWaterBody::AGZWaterBody()
{
    PrimaryActorTick.bCanEverTick = true;
    WaterMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("WaterMesh"));
    RootComponent = WaterMesh;
    // Create water plane
    const int32 Segments = 200;
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector2D> UVs;
    TArray<FVector> Normals;
    for (int32 y = 0; y <= Segments; y++)
    {
        for (int32 x = 0; x <= Segments; x++)
        {
            float XF = (float)x / Segments * WaterExtent.X - WaterExtent.X * 0.5f;
            float YF = (float)y / Segments * WaterExtent.Z - WaterExtent.Z * 0.5f;
            Vertices.Add(FVector(XF, 0, YF));
            UVs.Add(FVector2D((float)x / Segments, (float)y / Segments));
            Normals.Add(FVector::UpVector);
        }
    }
    for (int32 y = 0; y < Segments; y++)
    {
        for (int32 x = 0; x < Segments; x++)
        {
            int32 A = y * (Segments + 1) + x, B = A + Segments + 1;
            Triangles.Add(A); Triangles.Add(B); Triangles.Add(A + 1);
            Triangles.Add(B); Triangles.Add(B + 1); Triangles.Add(A + 1);
        }
    }
    WaterMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, {}, {}, false);
}

void AGZWaterBody::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    Time += DeltaTime;
    UpdateWaveAnimation(DeltaTime);
}

float AGZWaterBody::GetWaveHeight(FVector WorldLocation) const
{
    float W1 = FMath::Sin(WorldLocation.X * WaveFrequency + Time * 1.2f) * FMath::Cos(WorldLocation.Z * WaveFrequency * 1.5f + Time * 0.8f);
    float W2 = FMath::Sin(WorldLocation.X * WaveFrequency * 2.5f + Time * 1.8f) * FMath::Cos(WorldLocation.Z * WaveFrequency * 1.8f - Time * 1.1f);
    float W3 = FMath::Sin((WorldLocation.X + WorldLocation.Z) * WaveFrequency * 1.5f + Time * 0.6f);
    return (W1 * 0.6f + W2 * 0.35f + W3 * 0.45f) * WaveAmplitude;
}

void AGZWaterBody::UpdateWaveAnimation(float DeltaTime)
{
    // Update vertices for wave animation
    const int32 Segments = 200;
    const int32 VertCount = (Segments + 1) * (Segments + 1);
    TArray<FVector> Vertices;
    Vertices.SetNum(VertCount);
    for (int32 y = 0; y <= Segments; y++)
    {
        for (int32 x = 0; x <= Segments; x++)
        {
            float XF = (float)x / Segments * WaterExtent.X - WaterExtent.X * 0.5f;
            float YF = (float)y / Segments * WaterExtent.Z - WaterExtent.Z * 0.5f;
            float H = GetWaveHeight(FVector(XF, 0, YF));
            Vertices[y * (Segments + 1) + x] = FVector(XF, H, YF);
        }
    }
    WaterMesh->UpdateMeshSection(0, Vertices, {}, {}, {}, {});
}