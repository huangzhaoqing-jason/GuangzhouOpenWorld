#include "Network/EOSSystem/GZNetworkManager.h"
#include "GuangzhouOpenWorld.h"
#include "Math/UnrealMathUtility.h"

UGZNetworkManager::UGZNetworkManager()
{
}

void UGZNetworkManager::Initialize()
{
	Players.Empty();
	Players.Reserve(MaxPlayers);
	Bubbles.Empty();
	NextBubbleID = 0;
	BubbleUpdateTimer = 0.0f;
	EACCheckTimer = 0.0f;

	EACConfig.bKernelLevel = true;
	EACConfig.bMemoryIntegrityCheck = true;
	EACConfig.bSpeedHackDetection = true;
	EACConfig.bPositionIntegrityCheck = true;
	EACConfig.bInstantDisconnect = true;

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Network: EOS v%.2f, BubbleDynamic, %d players, merge<%.0fm, split>%.0fm, transition=%.1fs, EAC v%.2f (kernel-level, instant disconnect)"),
		EOSVersion, MaxPlayers, MergeDistance / 100.0f, SplitDistance / 100.0f, BubbleTransitionTime, EACVersion);
}

void UGZNetworkManager::Tick(float DeltaTime)
{
	BubbleUpdateTimer += DeltaTime;
	if (BubbleUpdateTimer >= BubbleUpdateInterval)
	{
		BubbleUpdateTimer = 0.0f;
		UpdateBubbles(DeltaTime);
	}

	EACCheckTimer += DeltaTime;
	if (EACCheckTimer >= EACCheckInterval)
	{
		EACCheckTimer = 0.0f;
		for (const FGZNetworkPlayer& Player : Players)
		{
			if (Player.bIsConnected)
			{
				RunEACCheck(Player.PlayerID);
			}
		}
	}
}

void UGZNetworkManager::RegisterPlayer(int32 PlayerID, const FVector& Position)
{
	if (Players.Num() >= MaxPlayers) return;

	FGZNetworkPlayer Player;
	Player.PlayerID = PlayerID;
	Player.Position = Position;
	Player.Rotation = FRotator::ZeroRotator;
	Player.Velocity = FVector::ZeroVector;
	Player.Health = 100.0f;
	Player.bIsConnected = true;
	Player.bIsServerAuthoritative = true;
	Players.Add(Player);

	FGZBubble Bubble;
	Bubble.BubbleID = NextBubbleID++;
	Bubble.Centroid = Position;
	Bubble.PlayerIDs.Add(PlayerID);
	Bubble.State = EGZBubbleState::Active;
	Bubble.TransitionProgress = 1.0f;
	Bubbles.Add(Bubble);

	Player.BubbleID = Bubble.BubbleID;
	Players.Last() = Player;

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Player %d registered, bubble %d created"), PlayerID, Bubble.BubbleID);
}

void UGZNetworkManager::UnregisterPlayer(int32 PlayerID)
{
	for (int32 i = Players.Num() - 1; i >= 0; --i)
	{
		if (Players[i].PlayerID == PlayerID)
		{
			int32 BubbleID = Players[i].BubbleID;
			Players.RemoveAt(i);

			for (FGZBubble& Bubble : Bubbles)
			{
				if (Bubble.BubbleID == BubbleID)
				{
					Bubble.PlayerIDs.Remove(PlayerID);
					if (Bubble.PlayerIDs.Num() == 0)
					{
						Bubbles.RemoveAll([BubbleID](const FGZBubble& B) { return B.BubbleID == BubbleID; });
					}
					break;
				}
			}
			break;
		}
	}
}

void UGZNetworkManager::UpdatePlayerPosition(int32 PlayerID, const FVector& NewPosition, const FRotator& NewRotation, const FVector& NewVelocity)
{
	for (FGZNetworkPlayer& Player : Players)
	{
		if (Player.PlayerID == PlayerID)
		{
			Player.Position = NewPosition;
			Player.Rotation = NewRotation;
			Player.Velocity = NewVelocity;
			break;
		}
	}
}

void UGZNetworkManager::SetPlayerHealth(int32 PlayerID, float Health)
{
	for (FGZNetworkPlayer& Player : Players)
	{
		if (Player.PlayerID == PlayerID)
		{
			Player.Health = FMath::Clamp(Health, 0.0f, 100.0f);
			break;
		}
	}
}

void UGZNetworkManager::SetPlayerBubble(int32 PlayerID, int32 BubbleID)
{
	for (FGZNetworkPlayer& Player : Players)
	{
		if (Player.PlayerID == PlayerID)
		{
			int32 OldBubbleID = Player.BubbleID;
			Player.BubbleID = BubbleID;

			for (FGZBubble& Bubble : Bubbles)
			{
				if (Bubble.BubbleID == OldBubbleID)
				{
					Bubble.PlayerIDs.Remove(PlayerID);
				}
				if (Bubble.BubbleID == BubbleID)
				{
					Bubble.PlayerIDs.AddUnique(PlayerID);
				}
			}
			break;
		}
	}
}

void UGZNetworkManager::EnableEAC(bool bEnable)
{
	if (!bEnable)
	{
		EACConfig.bKernelLevel = false;
		EACConfig.bMemoryIntegrityCheck = false;
		EACConfig.bSpeedHackDetection = false;
		EACConfig.bPositionIntegrityCheck = false;
		EACConfig.bInstantDisconnect = false;
	}
	else
	{
		EACConfig.bKernelLevel = true;
		EACConfig.bMemoryIntegrityCheck = true;
		EACConfig.bSpeedHackDetection = true;
		EACConfig.bPositionIntegrityCheck = true;
		EACConfig.bInstantDisconnect = true;
	}
}

void UGZNetworkManager::DisconnectCheater(int32 PlayerID, const FString& Reason)
{
	UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("EAC: Disconnecting player %d: %s"), PlayerID, *Reason);
	UnregisterPlayer(PlayerID);
}

void UGZNetworkManager::UpdateBubbles(float DeltaTime)
{
	for (FGZBubble& Bubble : Bubbles)
	{
		if (Bubble.State == EGZBubbleState::Merging || Bubble.State == EGZBubbleState::Split)
		{
			TransitionBubble(DeltaTime, Bubble);
		}
	}

	CheckMergeConditions(DeltaTime);
	CheckSplitConditions(DeltaTime);
}

void UGZNetworkManager::CheckMergeConditions(float DeltaTime)
{
	for (int32 i = 0; i < Bubbles.Num(); ++i)
	{
		for (int32 j = i + 1; j < Bubbles.Num(); ++j)
		{
			FGZBubble& A = Bubbles[i];
			FGZBubble& B = Bubbles[j];

			if (A.State != EGZBubbleState::Active || B.State != EGZBubbleState::Active) continue;

			float Dist = FVector::Dist(A.Centroid, B.Centroid);
			if (Dist < MergeDistance)
			{
				MergeBubbles(i, j);
				return;
			}
		}
	}
}

void UGZNetworkManager::CheckSplitConditions(float DeltaTime)
{
	for (FGZBubble& Bubble : Bubbles)
	{
		if (Bubble.State != EGZBubbleState::Active) continue;
		if (Bubble.PlayerIDs.Num() < 2) continue;

		FVector Centroid = CalculateBubbleCentroid(Bubble.PlayerIDs);
		bool bShouldSplit = false;

		for (int32 PlayerID : Bubble.PlayerIDs)
		{
			for (const FGZNetworkPlayer& Player : Players)
			{
				if (Player.PlayerID == PlayerID)
				{
					float Dist = FVector::Dist(Player.Position, Centroid);
					if (Dist > SplitDistance)
					{
						bShouldSplit = true;
						break;
					}
				}
			}
			if (bShouldSplit) break;
		}

		if (bShouldSplit)
		{
			SplitBubble(Bubble.BubbleID);
			return;
		}
	}
}

void UGZNetworkManager::MergeBubbles(int32 BubbleA, int32 BubbleB)
{
	if (BubbleA < 0 || BubbleA >= Bubbles.Num() || BubbleB < 0 || BubbleB >= Bubbles.Num()) return;

	FGZBubble& A = Bubbles[BubbleA];
	FGZBubble& B = Bubbles[BubbleB];

	A.State = EGZBubbleState::Merging;
	A.TransitionProgress = 0.0f;
	A.TargetCentroid = (A.Centroid * A.PlayerIDs.Num() + B.Centroid * B.PlayerIDs.Num()) / (A.PlayerIDs.Num() + B.PlayerIDs.Num());
	A.TargetPlayerIDs = A.PlayerIDs;
	A.TargetPlayerIDs.Append(B.PlayerIDs);

	for (int32 PlayerID : B.PlayerIDs)
	{
		SetPlayerBubble(PlayerID, A.BubbleID);
	}

	B.PlayerIDs.Empty();
	B.State = EGZBubbleState::Idle;
	Bubbles.RemoveAt(BubbleB);
}

void UGZNetworkManager::SplitBubble(int32 BubbleID)
{
	for (int32 i = 0; i < Bubbles.Num(); ++i)
	{
		if (Bubbles[i].BubbleID == BubbleID)
		{
			FGZBubble& OldBubble = Bubbles[i];
			OldBubble.State = EGZBubbleState::Split;
			OldBubble.TransitionProgress = 0.0f;

			TArray<int32> Group1, Group2;
			FVector Centroid = CalculateBubbleCentroid(OldBubble.PlayerIDs);

			for (int32 PlayerID : OldBubble.PlayerIDs)
			{
				for (const FGZNetworkPlayer& Player : Players)
				{
					if (Player.PlayerID == PlayerID)
					{
						if (FVector::Dist(Player.Position, Centroid) < SplitDistance * 0.5f)
						{
							Group1.Add(PlayerID);
						}
						else
						{
							Group2.Add(PlayerID);
						}
						break;
					}
				}
			}

			if (Group2.Num() > 0)
			{
				FGZBubble NewBubble;
				NewBubble.BubbleID = NextBubbleID++;
				NewBubble.Centroid = CalculateBubbleCentroid(Group2);
				NewBubble.PlayerIDs = Group2;
				NewBubble.State = EGZBubbleState::Active;
				NewBubble.TransitionProgress = 1.0f;
				Bubbles.Add(NewBubble);

				for (int32 PlayerID : Group2)
				{
					SetPlayerBubble(PlayerID, NewBubble.BubbleID);
				}
			}

			OldBubble.PlayerIDs = Group1;
			OldBubble.Centroid = CalculateBubbleCentroid(Group1);
			OldBubble.State = EGZBubbleState::Active;
			OldBubble.TransitionProgress = 1.0f;
			return;
		}
	}
}

void UGZNetworkManager::TransitionBubble(float DeltaTime, FGZBubble& Bubble)
{
	Bubble.TransitionProgress += DeltaTime / BubbleTransitionTime;

	if (Bubble.TransitionProgress >= 1.0f)
	{
		Bubble.TransitionProgress = 1.0f;
		Bubble.State = EGZBubbleState::Active;

		if (Bubble.State == EGZBubbleState::Merging)
		{
			Bubble.Centroid = Bubble.TargetCentroid;
			Bubble.PlayerIDs = Bubble.TargetPlayerIDs;
			Bubble.TargetPlayerIDs.Empty();
		}
	}
	else
	{
		float Alpha = Bubble.TransitionProgress;
		Bubble.Centroid = FMath::Lerp(Bubble.Centroid, Bubble.TargetCentroid, Alpha);
	}
}

FVector UGZNetworkManager::CalculateBubbleCentroid(const TArray<int32>& PlayerIDs) const
{
	FVector Sum = FVector::ZeroVector;
	int32 Count = 0;

	for (int32 PlayerID : PlayerIDs)
	{
		for (const FGZNetworkPlayer& Player : Players)
		{
			if (Player.PlayerID == PlayerID)
			{
				Sum += Player.Position;
				Count++;
				break;
			}
		}
	}

	return Count > 0 ? Sum / Count : FVector::ZeroVector;
}

void UGZNetworkManager::ReplicateStateToPlayers()
{
}

void UGZNetworkManager::ValidatePlayerState(int32 PlayerID)
{
	RunEACCheck(PlayerID);
}

void UGZNetworkManager::RunEACCheck(int32 PlayerID)
{
	if (!EACConfig.bKernelLevel) return;

	FGZNetworkPlayer* TargetPlayer = nullptr;
	for (FGZNetworkPlayer& Player : Players)
	{
		if (Player.PlayerID == PlayerID)
		{
			TargetPlayer = &Player;
			break;
		}
	}

	if (!TargetPlayer) return;

	if (EACConfig.bSpeedHackDetection)
	{
		float Speed = TargetPlayer->Velocity.Size();
		if (Speed > MaxPlayerSpeed)
		{
			DisconnectCheater(PlayerID, TEXT("Speed hack detected"));
			return;
		}
	}

	if (EACConfig.bPositionIntegrityCheck)
	{
		static TMap<int32, FVector> LastPositions;
		if (LastPositions.Contains(PlayerID))
		{
			float Jump = FVector::Dist(TargetPlayer->Position, LastPositions[PlayerID]);
			if (Jump > MaxPlayerPositionJump)
			{
				DisconnectCheater(PlayerID, TEXT("Position teleport detected"));
				LastPositions.Remove(PlayerID);
				return;
			}
		}
		LastPositions.Add(PlayerID, TargetPlayer->Position);
	}
}