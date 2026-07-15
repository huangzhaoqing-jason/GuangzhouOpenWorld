#include "Game/Gameplay/GZVerticalPathSystem.h"

UGZVerticalPathSystem::UGZVerticalPathSystem()
{
}

void UGZVerticalPathSystem::Initialize()
{
	Nodes.Empty();
	GuidanceCues.Empty();
	NextNodeId = 1;
}

int32 UGZVerticalPathSystem::RegisterNode(FVector Location, EGZVerticalLayer Layer, bool bIsCover, bool bIsClimbable, float GuidanceWeight)
{
	FGZVerticalNode Node;
	Node.NodeId = NextNodeId++;
	Node.Location = Location;
	Node.Layer = Layer;
	Node.bIsCover = bIsCover;
	Node.bIsClimbable = bIsClimbable;
	Node.GuidanceWeight = GuidanceWeight;
	Nodes.Add(Node);
	return Node.NodeId;
}

void UGZVerticalPathSystem::ConnectNodes(int32 A, int32 B)
{
	FGZVerticalNode* NodeA = const_cast<FGZVerticalNode*>(FindNode(A));
	FGZVerticalNode* NodeB = const_cast<FGZVerticalNode*>(FindNode(B));
	if (NodeA && NodeB)
	{
		if (!NodeA->ConnectedIds.Contains(B))
		{
			NodeA->ConnectedIds.Add(B);
		}
		if (!NodeB->ConnectedIds.Contains(A))
		{
			NodeB->ConnectedIds.Add(A);
		}
	}
}

FGZVerticalPath UGZVerticalPathSystem::FindPath(FVector StartLocation, EGZVerticalLayer TargetLayer) const
{
	FGZVerticalPath Result;

	int32 BestTargetId = INDEX_NONE;
	float BestDist = MAX_FLT;
	for (const FGZVerticalNode& Node : Nodes)
	{
		if (Node.Layer == TargetLayer)
		{
			const float Dist = FVector::DistSquared(StartLocation, Node.Location);
			if (Dist < BestDist)
			{
				BestDist = Dist;
				BestTargetId = Node.NodeId;
			}
		}
	}

	if (BestTargetId != INDEX_NONE)
	{
		Result = FindPathToNode(StartLocation, BestTargetId);
	}

	return Result;
}

FGZVerticalPath UGZVerticalPathSystem::FindPathToNode(FVector StartLocation, int32 TargetNodeId) const
{
	FGZVerticalPath Result;

	const FGZVerticalNode* TargetNode = FindNode(TargetNodeId);
	if (!TargetNode)
	{
		return Result;
	}

	int32 StartId = FindNearestNode(StartLocation, 5000.0f, EGZVerticalLayer::Street);
	if (StartId == INDEX_NONE)
	{
		StartId = FindNearestNode(StartLocation, 5000.0f, EGZVerticalLayer::Arcade);
	}
	if (StartId == INDEX_NONE)
	{
		return Result;
	}

	// A*
	TMap<int32, float> GScore;
	TMap<int32, float> FScore;
	TMap<int32, int32> CameFrom;
	TArray<int32> OpenSet;

	GScore.Add(StartId, 0.0f);
	FScore.Add(StartId, HeuristicCost(*FindNode(StartId), *TargetNode));
	OpenSet.Add(StartId);

	while (OpenSet.Num() > 0)
	{
		int32 CurrentId = INDEX_NONE;
		float LowestF = MAX_FLT;
		for (int32 Id : OpenSet)
		{
			const float* F = FScore.Find(Id);
			if (F && *F < LowestF)
			{
				LowestF = *F;
				CurrentId = Id;
			}
		}

		if (CurrentId == INDEX_NONE)
		{
			break;
		}

		if (CurrentId == TargetNodeId)
		{
			// Reconstruct path
			TArray<int32> Reversed;
			int32 Step = TargetNodeId;
			while (Step != StartId)
			{
				Reversed.Add(Step);
				const int32* Prev = CameFrom.Find(Step);
				if (!Prev)
				{
					break;
				}
				Step = *Prev;
			}
			Reversed.Add(StartId);

			for (int32 i = Reversed.Num() - 1; i >= 0; --i)
			{
				Result.NodeIds.Add(Reversed[i]);
			}

			for (int32 i = 0; i < Result.NodeIds.Num() - 1; ++i)
			{
				const FGZVerticalNode* A = FindNode(Result.NodeIds[i]);
				const FGZVerticalNode* B = FindNode(Result.NodeIds[i + 1]);
				if (A && B)
				{
					Result.TotalLength += FVector::Dist(A->Location, B->Location);
				}
			}
			return Result;
		}

		OpenSet.Remove(CurrentId);

		const FGZVerticalNode* CurrentNode = FindNode(CurrentId);
		if (!CurrentNode)
		{
			continue;
		}

		const float CurrentG = GScore.FindRef(CurrentId);

		for (int32 NeighborId : CurrentNode->ConnectedIds)
		{
			const FGZVerticalNode* Neighbor = FindNode(NeighborId);
			if (!Neighbor)
			{
				continue;
			}

			const float EdgeCost = FVector::Dist(CurrentNode->Location, Neighbor->Location);
			const float TentativeG = CurrentG + EdgeCost;

			const float* ExistingG = GScore.Find(NeighborId);
			if (!ExistingG || TentativeG < *ExistingG)
			{
				CameFrom.Add(NeighborId, CurrentId);
				GScore.Add(NeighborId, TentativeG);
				FScore.Add(NeighborId, TentativeG + HeuristicCost(*Neighbor, *TargetNode));
				if (!OpenSet.Contains(NeighborId))
				{
					OpenSet.Add(NeighborId);
				}
			}
		}
	}

	return Result;
}

int32 UGZVerticalPathSystem::FindNearestNode(FVector Location, float MaxRadius, EGZVerticalLayer Layer) const
{
	int32 BestId = INDEX_NONE;
	float BestDistSq = FMath::Square(MaxRadius);

	for (const FGZVerticalNode& Node : Nodes)
	{
		if (Node.Layer != Layer)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(Location, Node.Location);
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestId = Node.NodeId;
		}
	}

	return BestId;
}

TArray<int32> UGZVerticalPathSystem::QueryCoverNodes(FVector Location, float Radius) const
{
	TArray<int32> Result;
	const float RadiusSq = FMath::Square(Radius);

	for (const FGZVerticalNode& Node : Nodes)
	{
		if (Node.bIsCover && FVector::DistSquared(Location, Node.Location) < RadiusSq)
		{
			Result.Add(Node.NodeId);
		}
	}

	return Result;
}

void UGZVerticalPathSystem::AddGuidanceCue(EGZGuidanceType Type, FVector Origin, FVector Direction, float Radius)
{
	FGZGuidanceCue Cue;
	Cue.Type = Type;
	Cue.Origin = Origin;
	Cue.Direction = Direction.GetSafeNormal();
	Cue.Radius = Radius;
	Cue.bActive = true;
	GuidanceCues.Add(Cue);
}

FVector UGZVerticalPathSystem::GetGuidanceDirection(FVector Location) const
{
	FVector WeightedDir = FVector::ZeroVector;
	float TotalWeight = 0.0f;

	for (const FGZGuidanceCue& Cue : GuidanceCues)
	{
		if (!Cue.bActive)
		{
			continue;
		}

		const float Dist = FVector::Dist(Location, Cue.Origin);
		if (Dist < Cue.Radius)
		{
			const float Falloff = 1.0f - FMath::Clamp(Dist / Cue.Radius, 0.0f, 1.0f);
			WeightedDir += Cue.Direction * Falloff;
			TotalWeight += Falloff;
		}
	}

	if (TotalWeight > KINDA_SMALL_NUMBER)
	{
		return (WeightedDir / TotalWeight).GetSafeNormal();
	}

	return FVector::ZeroVector;
}

const FGZVerticalNode* UGZVerticalPathSystem::FindNode(int32 NodeId) const
{
	return Nodes.FindByPredicate([NodeId](const FGZVerticalNode& N) { return N.NodeId == NodeId; });
}

float UGZVerticalPathSystem::HeuristicCost(const FGZVerticalNode& A, const FGZVerticalNode& B) const
{
	return FVector::Dist(A.Location, B.Location);
}
