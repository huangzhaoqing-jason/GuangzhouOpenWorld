#pragma once

#include "CoreMinimal.h"
#include "Game/GZMissionManager.h"

/**
 * Loads Docs/Data/missions_act1.json (or Content/Data copy) into FGZMissionData.
 * No UE editor required to author JSON; call when GameInstance boots (M1b+).
 */
struct GUANGZHOUOPENWORLD_API FGZMissionJsonLoader
{
	/** Returns number of missions registered. 0 if file missing/invalid. */
	static int32 LoadAct1Into(UGZMissionManager& MissionManager);

	static EGZMissionType ParseType(const FString& Type);
	static EGZMissionStatus ParseStatus(const FString& Status);
};
