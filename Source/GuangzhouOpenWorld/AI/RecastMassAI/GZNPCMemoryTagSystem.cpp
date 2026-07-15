#include "AI/RecastMassAI/GZNPCMemoryTagSystem.h"

UGZNPCMemoryTagSystem::UGZNPCMemoryTagSystem()
{
}

void UGZNPCMemoryTagSystem::Initialize()
{
	NPCMemories.Empty();
	EscalationRules.Empty();
	RegisterEscalationRules();
}

void UGZNPCMemoryTagSystem::RegisterNPC(int32 NPCId, FVector Location)
{
	if (FindMemory(NPCId))
	{
		return;
	}

	FGZNPCMemory Memory;
	Memory.NPCId = NPCId;
	Memory.Location = Location;
	Memory.CurrentStage = EGZNPCConflictStage::Calm;
	NPCMemories.Add(Memory);
}

void UGZNPCMemoryTagSystem::UnregisterNPC(int32 NPCId)
{
	NPCMemories.RemoveAll([NPCId](const FGZNPCMemory& M) { return M.NPCId == NPCId; });
}

void UGZNPCMemoryTagSystem::SetNPCLocation(int32 NPCId, FVector Location)
{
	FGZNPCMemory* Memory = FindMemory(NPCId);
	if (Memory)
	{
		Memory->Location = Location;
	}
}

void UGZNPCMemoryTagSystem::AddTag(int32 NPCId, EGZNPCConflictTag TagType, float Intensity, float CurrentTime)
{
	FGZNPCMemory* Memory = FindMemory(NPCId);
	if (!Memory)
	{
		return;
	}

	const FGZNPCEscalationRule* Rule = FindRule(TagType);
	const float Decay = Rule ? Rule->DecayPerSecond : 5.0f;

	FGZNPCTagInstance* Existing = Memory->Tags.FindByPredicate([TagType](const FGZNPCTagInstance& T) { return T.TagType == TagType; });
	if (Existing)
	{
		Existing->Intensity = FMath::Clamp(Existing->Intensity + Intensity, 0.0f, 150.0f);
		Existing->LastTriggeredTime = CurrentTime;
	}
	else
	{
		FGZNPCTagInstance NewTag;
		NewTag.TagType = TagType;
		NewTag.Intensity = FMath::Clamp(Intensity, 0.0f, 150.0f);
		NewTag.CreatedTime = CurrentTime;
		NewTag.LastTriggeredTime = CurrentTime;
		NewTag.DecayPerSecond = Decay;
		Memory->Tags.Add(NewTag);
	}

	const EGZNPCConflictStage OldStage = Memory->CurrentStage;
	Memory->CurrentStage = ComputeStage(*Memory);
	if (OldStage != Memory->CurrentStage)
	{
		OnNPCStageChanged.Broadcast(NPCId, Memory->CurrentStage, OldStage);
	}
}

void UGZNPCMemoryTagSystem::TickMemory(float DeltaSeconds, float CurrentTime)
{
	for (FGZNPCMemory& Memory : NPCMemories)
	{
		const EGZNPCConflictStage OldStage = Memory.CurrentStage;
		DecayTags(Memory, DeltaSeconds);
		Memory.CurrentStage = ComputeStage(Memory);
		if (OldStage != Memory.CurrentStage)
		{
			OnNPCStageChanged.Broadcast(Memory.NPCId, Memory.CurrentStage, OldStage);
		}
	}

	PropagateCrowdReaction(CurrentTime);
}

EGZNPCConflictStage UGZNPCMemoryTagSystem::GetNPCStage(int32 NPCId) const
{
	const FGZNPCMemory* Memory = FindMemoryRead(NPCId);
	return Memory ? Memory->CurrentStage : EGZNPCConflictStage::Calm;
}

float UGZNPCMemoryTagSystem::GetTagIntensity(int32 NPCId, EGZNPCConflictTag TagType) const
{
	const FGZNPCMemory* Memory = FindMemoryRead(NPCId);
	if (!Memory)
	{
		return 0.0f;
	}

	const FGZNPCTagInstance* Tag = Memory->Tags.FindByPredicate([TagType](const FGZNPCTagInstance& T) { return T.TagType == TagType; });
	return Tag ? Tag->Intensity : 0.0f;
}

void UGZNPCMemoryTagSystem::RegisterEscalationRules()
{
	auto AddRule = [&](EGZNPCConflictTag Tag, float Annoyed, float Angry, float Hostile,
		float Decay, float PropagationIntensity, float PropagationRadius)
	{
		FGZNPCEscalationRule Rule;
		Rule.TagType = Tag;
		Rule.AnnoyedThreshold = Annoyed;
		Rule.AngryThreshold = Angry;
		Rule.HostileThreshold = Hostile;
		Rule.DecayPerSecond = Decay;
		Rule.CrowdPropagationIntensity = PropagationIntensity;
		Rule.CrowdPropagationRadius = PropagationRadius;
		EscalationRules.Add(Rule);
	};

	// 撞摊：强度高、传播快；鸣笛：低强度但易传播；拥挤奔跑：低强度但范围大
	AddRule(EGZNPCConflictTag::VendorDisturbed,     25.0f,  60.0f, 100.0f,  4.0f, 15.0f, 600.0f);
	AddRule(EGZNPCConflictTag::HonkedAt,            30.0f,  65.0f, 110.0f,  8.0f, 10.0f, 450.0f);
	AddRule(EGZNPCConflictTag::NearCollision,       35.0f,  75.0f, 120.0f,  7.0f, 12.0f, 500.0f);
	AddRule(EGZNPCConflictTag::PlayerBlockingPath,  20.0f,  50.0f,  90.0f,  5.0f,  8.0f, 400.0f);
	AddRule(EGZNPCConflictTag::PlayerSprintingCrowd, 15.0f, 40.0f,  70.0f, 10.0f,  5.0f, 350.0f);
	AddRule(EGZNPCConflictTag::Sympathy,            20.0f,  45.0f,  80.0f,  6.0f,  6.0f, 400.0f);
}

const FGZNPCEscalationRule* UGZNPCMemoryTagSystem::FindRule(EGZNPCConflictTag TagType) const
{
	return EscalationRules.FindByPredicate([TagType](const FGZNPCEscalationRule& R) { return R.TagType == TagType; });
}

FGZNPCMemory* UGZNPCMemoryTagSystem::FindMemory(int32 NPCId)
{
	return NPCMemories.FindByPredicate([NPCId](const FGZNPCMemory& M) { return M.NPCId == NPCId; });
}

const FGZNPCMemory* UGZNPCMemoryTagSystem::FindMemoryRead(int32 NPCId) const
{
	return NPCMemories.FindByPredicate([NPCId](const FGZNPCMemory& M) { return M.NPCId == NPCId; });
}

EGZNPCConflictStage UGZNPCMemoryTagSystem::ComputeStage(const FGZNPCMemory& Memory) const
{
	float MaxIntensity = 0.0f;
	for (const FGZNPCTagInstance& Tag : Memory.Tags)
	{
		const FGZNPCEscalationRule* Rule = FindRule(Tag.TagType);
		if (!Rule)
		{
			continue;
		}
		MaxIntensity = FMath::Max(MaxIntensity, Tag.Intensity);
	}

	for (const FGZNPCTagInstance& Tag : Memory.Tags)
	{
		const FGZNPCEscalationRule* Rule = FindRule(Tag.TagType);
		if (!Rule)
		{
			continue;
		}
		if (Tag.Intensity >= Rule->HostileThreshold)
		{
			return EGZNPCConflictStage::Hostile;
		}
	}

	for (const FGZNPCTagInstance& Tag : Memory.Tags)
	{
		const FGZNPCEscalationRule* Rule = FindRule(Tag.TagType);
		if (!Rule)
		{
			continue;
		}
		if (Tag.Intensity >= Rule->AngryThreshold)
		{
			return EGZNPCConflictStage::Angry;
		}
	}

	for (const FGZNPCTagInstance& Tag : Memory.Tags)
	{
		const FGZNPCEscalationRule* Rule = FindRule(Tag.TagType);
		if (!Rule)
		{
			continue;
		}
		if (Tag.Intensity >= Rule->AnnoyedThreshold)
		{
			return EGZNPCConflictStage::Annoyed;
		}
	}

	return EGZNPCConflictStage::Calm;
}

void UGZNPCMemoryTagSystem::DecayTags(FGZNPCMemory& Memory, float DeltaSeconds)
{
	for (int32 i = Memory.Tags.Num() - 1; i >= 0; --i)
	{
		FGZNPCTagInstance& Tag = Memory.Tags[i];
		Tag.Intensity -= Tag.DecayPerSecond * DeltaSeconds;
		if (Tag.Intensity <= 0.0f)
		{
			Memory.Tags.RemoveAt(i);
		}
	}
}

void UGZNPCMemoryTagSystem::PropagateCrowdReaction(float CurrentTime)
{
	for (const FGZNPCMemory& Source : NPCMemories)
	{
		if (Source.CurrentStage != EGZNPCConflictStage::Angry && Source.CurrentStage != EGZNPCConflictStage::Hostile)
		{
			continue;
		}

		for (FGZNPCMemory& Target : NPCMemories)
		{
			if (Source.NPCId == Target.NPCId)
			{
				continue;
			}

			float MaxRadius = 0.0f;
			float MaxIntensity = 0.0f;
			for (const FGZNPCTagInstance& Tag : Source.Tags)
			{
				const FGZNPCEscalationRule* Rule = FindRule(Tag.TagType);
				if (Rule)
				{
					MaxRadius = FMath::Max(MaxRadius, Rule->CrowdPropagationRadius);
					MaxIntensity = FMath::Max(MaxIntensity, Rule->CrowdPropagationIntensity);
				}
			}

			if (MaxRadius <= 0.0f)
			{
				continue;
			}

			const float Dist = FVector::Dist(Source.Location, Target.Location);
			if (Dist < MaxRadius)
			{
				const float Falloff = 1.0f - FMath::Clamp(Dist / MaxRadius, 0.0f, 1.0f);
				AddTag(Target.NPCId, EGZNPCConflictTag::Sympathy, MaxIntensity * Falloff, CurrentTime);
				OnCrowdReactionTriggered.Broadcast(Source.NPCId, Target.NPCId);
			}
		}
	}
}
