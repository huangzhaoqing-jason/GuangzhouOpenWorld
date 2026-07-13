#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZNetworkManager.generated.h"

UENUM(BlueprintType)
enum class EGZBubbleState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Merging		UMETA(DisplayName = "Merging"),
	Split		UMETA(DisplayName = "Split"),
	Active		UMETA(DisplayName = "Active"),
};

USTRUCT(BlueprintType)
struct FGZBubble
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BubbleID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Centroid = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> PlayerIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZBubbleState State = EGZBubbleState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TransitionProgress = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetCentroid = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> TargetPlayerIDs;
};

USTRUCT(BlueprintType)
struct FGZNetworkPlayer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BubbleID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsConnected = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsServerAuthoritative = true;
};

USTRUCT(BlueprintType)
struct FGZEACConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bKernelLevel = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMemoryIntegrityCheck = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSpeedHackDetection = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPositionIntegrityCheck = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInstantDisconnect = true;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZNetworkManager : public UObject
{
	GENERATED_BODY()

public:
	UGZNetworkManager();

	void Initialize();
	void Tick(float DeltaTime);
	void RegisterPlayer(int32 PlayerID, const FVector& Position);
	void UnregisterPlayer(int32 PlayerID);
	void UpdatePlayerPosition(int32 PlayerID, const FVector& NewPosition, const FRotator& NewRotation, const FVector& NewVelocity);
	void SetPlayerHealth(int32 PlayerID, float Health);
	void SetPlayerBubble(int32 PlayerID, int32 BubbleID);

	UFUNCTION(BlueprintPure)
	int32 GetPlayerCount() const { return Players.Num(); }

	UFUNCTION(BlueprintPure)
	int32 GetBubbleCount() const { return Bubbles.Num(); }

	UFUNCTION(BlueprintPure)
	const FGZEACConfig& GetEACConfig() const { return EACConfig; }

	UFUNCTION(BlueprintCallable, Category = "EAC")
	void EnableEAC(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "EAC")
	void DisconnectCheater(int32 PlayerID, const FString& Reason);

private:
	void UpdateBubbles(float DeltaTime);
	void CheckMergeConditions(float DeltaTime);
	void CheckSplitConditions(float DeltaTime);
	void MergeBubbles(int32 BubbleA, int32 BubbleB);
	void SplitBubble(int32 BubbleID);
	void TransitionBubble(float DeltaTime, FGZBubble& Bubble);
	FVector CalculateBubbleCentroid(const TArray<int32>& PlayerIDs) const;
	void ReplicateStateToPlayers();
	void ValidatePlayerState(int32 PlayerID);
	void RunEACCheck(int32 PlayerID);

	UPROPERTY()
	TArray<FGZNetworkPlayer> Players;

	UPROPERTY()
	TArray<FGZBubble> Bubbles;

	UPROPERTY()
	FGZEACConfig EACConfig;

	int32 MaxPlayers = 64;
	int32 NextBubbleID = 0;
	float BubbleUpdateTimer = 0.0f;
	float EACCheckTimer = 0.0f;

	static constexpr float EOSVersion = 1.190f;
	static constexpr float EACVersion = 1.98f;
	static constexpr float MergeDistance = 10000.0f;
	static constexpr float SplitDistance = 15000.0f;
	static constexpr float BubbleTransitionTime = 0.1f;
	static constexpr float BubbleUpdateInterval = 0.5f;
	static constexpr float EACCheckInterval = 0.1f;
	static constexpr float MaxPlayerSpeed = 5000.0f;
	static constexpr float MaxPlayerPositionJump = 10000.0f;
};