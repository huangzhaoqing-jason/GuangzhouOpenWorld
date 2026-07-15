#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZVerticalPathSystem.generated.h"

UENUM(BlueprintType)
enum class EGZVerticalLayer : uint8
{
	Street    UMETA(DisplayName = "Street"),
	Arcade    UMETA(DisplayName = "Arcade"),
	Balcony   UMETA(DisplayName = "Balcony"),
	Rooftop   UMETA(DisplayName = "Rooftop"),
	Alley     UMETA(DisplayName = "Alley"),
};

UENUM(BlueprintType)
enum class EGZGuidanceType : uint8
{
	Wind      UMETA(DisplayName = "Wind"),
	Light     UMETA(DisplayName = "Light"),
};

USTRUCT()
struct FGZVerticalNode
{
	GENERATED_BODY()

	UPROPERTY()
	int32 NodeId = 0;

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	EGZVerticalLayer Layer = EGZVerticalLayer::Street;

	UPROPERTY()
	TArray<int32> ConnectedIds;

	UPROPERTY()
	bool bIsCover = false;

	UPROPERTY()
	bool bIsClimbable = false;

	UPROPERTY()
	float GuidanceWeight = 1.0f;
};

USTRUCT()
struct FGZVerticalPath
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<int32> NodeIds;

	UPROPERTY()
	float TotalLength = 0.0f;
};

USTRUCT()
struct FGZGuidanceCue
{
	GENERATED_BODY()

	UPROPERTY()
	EGZGuidanceType Type = EGZGuidanceType::Wind;

	UPROPERTY()
	FVector Origin = FVector::ZeroVector;

	UPROPERTY()
	FVector Direction = FVector::ForwardVector;

	UPROPERTY()
	float Radius = 1000.0f;

	UPROPERTY()
	bool bActive = true;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZVerticalPathSystem : public UObject
{
	GENERATED_BODY()

public:
	UGZVerticalPathSystem();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	int32 RegisterNode(FVector Location, EGZVerticalLayer Layer, bool bIsCover, bool bIsClimbable, float GuidanceWeight);

	UFUNCTION(BlueprintCallable)
	void ConnectNodes(int32 A, int32 B);

	UFUNCTION(BlueprintCallable)
	FGZVerticalPath FindPath(FVector StartLocation, EGZVerticalLayer TargetLayer) const;

	UFUNCTION(BlueprintCallable)
	FGZVerticalPath FindPathToNode(FVector StartLocation, int32 TargetNodeId) const;

	UFUNCTION(BlueprintCallable)
	int32 FindNearestNode(FVector Location, float MaxRadius, EGZVerticalLayer Layer) const;

	UFUNCTION(BlueprintCallable)
	TArray<int32> QueryCoverNodes(FVector Location, float Radius) const;

	UFUNCTION(BlueprintCallable)
	void AddGuidanceCue(EGZGuidanceType Type, FVector Origin, FVector Direction, float Radius);

	UFUNCTION(BlueprintCallable)
	FVector GetGuidanceDirection(FVector Location) const;

	UFUNCTION(BlueprintPure)
	const TArray<FGZVerticalNode>& GetNodes() const { return Nodes; }

	UFUNCTION(BlueprintPure)
	const TArray<FGZGuidanceCue>& GetGuidanceCues() const { return GuidanceCues; }

private:
	const FGZVerticalNode* FindNode(int32 NodeId) const;

	float HeuristicCost(const FGZVerticalNode& A, const FGZVerticalNode& B) const;

	UPROPERTY()
	TArray<FGZVerticalNode> Nodes;

	UPROPERTY()
	TArray<FGZGuidanceCue> GuidanceCues;

	int32 NextNodeId = 1;
};
