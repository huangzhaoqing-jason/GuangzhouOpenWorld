#include "Game/GZSaveGame.h"

UGZSaveGame::UGZSaveGame()
{
	SaveSlotName = TEXT("AutoSave");
	SaveDateTime = FDateTime::Now();
	SaveDisplayName = TEXT("New Save");
}