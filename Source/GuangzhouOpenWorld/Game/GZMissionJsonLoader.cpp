#include "Game/GZMissionJsonLoader.h"
#include "GuangzhouOpenWorld.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

EGZMissionType FGZMissionJsonLoader::ParseType(const FString& Type)
{
	if (Type.Equals(TEXT("MainStory"), ESearchCase::IgnoreCase)) return EGZMissionType::MainStory;
	if (Type.Equals(TEXT("Wanted"), ESearchCase::IgnoreCase)) return EGZMissionType::Wanted;
	if (Type.Equals(TEXT("Daily"), ESearchCase::IgnoreCase)) return EGZMissionType::Daily;
	if (Type.Equals(TEXT("Racing"), ESearchCase::IgnoreCase)) return EGZMissionType::Racing;
	if (Type.Equals(TEXT("Collection"), ESearchCase::IgnoreCase)) return EGZMissionType::Collection;
	return EGZMissionType::Side;
}

EGZMissionStatus FGZMissionJsonLoader::ParseStatus(const FString& Status)
{
	if (Status.Equals(TEXT("Locked"), ESearchCase::IgnoreCase)) return EGZMissionStatus::Locked;
	if (Status.Equals(TEXT("Active"), ESearchCase::IgnoreCase)) return EGZMissionStatus::Active;
	if (Status.Equals(TEXT("Completed"), ESearchCase::IgnoreCase)) return EGZMissionStatus::Completed;
	if (Status.Equals(TEXT("Failed"), ESearchCase::IgnoreCase)) return EGZMissionStatus::Failed;
	return EGZMissionStatus::Available;
}

int32 FGZMissionJsonLoader::LoadAct1Into(UGZMissionManager& MissionManager)
{
	TArray<FString> Candidates;
	Candidates.Add(FPaths::ProjectContentDir() / TEXT("Data/missions_act1.json"));
	Candidates.Add(FPaths::ProjectDir() / TEXT("Docs/Data/missions_act1.json"));

	FString JsonText;
	FString UsedPath;
	for (const FString& Path : Candidates)
	{
		if (FFileHelper::LoadFileToString(JsonText, *Path))
		{
			UsedPath = Path;
			break;
		}
	}

	if (UsedPath.IsEmpty())
	{
		UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("GZMissionJsonLoader: missions_act1.json not found under Content/Data or Docs/Data"));
		return 0;
	}

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		UE_LOG(LogGuangzhouOpenWorld, Error, TEXT("GZMissionJsonLoader: failed to parse %s"), *UsedPath);
		return 0;
	}

	const TArray<TSharedPtr<FJsonValue>>* MissionsArray = nullptr;
	if (!Root->TryGetArrayField(TEXT("missions"), MissionsArray) || !MissionsArray)
	{
		UE_LOG(LogGuangzhouOpenWorld, Error, TEXT("GZMissionJsonLoader: missing missions array in %s"), *UsedPath);
		return 0;
	}

	int32 Loaded = 0;
	for (const TSharedPtr<FJsonValue>& Value : *MissionsArray)
	{
		const TSharedPtr<FJsonObject> Obj = Value->AsObject();
		if (!Obj.IsValid())
		{
			continue;
		}

		FGZMissionData Mission;
		Mission.MissionID = FName(*Obj->GetStringField(TEXT("id")));
		Mission.MissionName = Obj->GetStringField(TEXT("title"));
		Mission.Description = Obj->GetStringField(TEXT("district_id"));
		Mission.MissionType = ParseType(Obj->GetStringField(TEXT("type")));
		Mission.Status = ParseStatus(Obj->GetStringField(TEXT("status")));

		if (Obj->HasField(TEXT("reward")))
		{
			const TSharedPtr<FJsonObject> Reward = Obj->GetObjectField(TEXT("reward"));
			if (Reward.IsValid())
			{
				Mission.RewardMoney = static_cast<float>(Reward->GetNumberField(TEXT("cash")));
				if (Reward->HasField(TEXT("item")))
				{
					Mission.RewardItem = Reward->GetStringField(TEXT("item"));
				}
			}
		}

		const TArray<TSharedPtr<FJsonValue>>* Objectives = nullptr;
		if (Obj->TryGetArrayField(TEXT("objectives"), Objectives) && Objectives)
		{
			for (const TSharedPtr<FJsonValue>& ObjVal : *Objectives)
			{
				const TSharedPtr<FJsonObject> O = ObjVal->AsObject();
				if (!O.IsValid())
				{
					continue;
				}
				FGZMissionObjective Objective;
				Objective.Description = O->GetStringField(TEXT("description"));
				Objective.TargetCount = static_cast<int32>(O->GetNumberField(TEXT("targetCount")));
				Mission.Objectives.Add(Objective);
			}
		}

		MissionManager.RegisterMission(Mission);
		++Loaded;
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("GZMissionJsonLoader: registered %d missions from %s"), Loaded, *UsedPath);
	return Loaded;
}
