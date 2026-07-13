#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GZMainMenuWidget.generated.h"

UENUM(BlueprintType)
enum class EGZMainMenuTab : uint8
{
	Continue	UMETA(DisplayName = "Continue"),
	SinglePlayer UMETA(DisplayName = "Single Player"),
	Multiplayer	UMETA(DisplayName = "Multiplayer"),
	Settings	UMETA(DisplayName = "Settings"),
	Quit		UMETA(DisplayName = "Quit"),
};

UCLASS()
class GUANGZHOUOPENWORLD_API UGZMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGZMainMenuWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void SetActiveTab(EGZMainMenuTab Tab);

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnContinueGame();

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnNewGame();

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnLoadGame(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnMultiplayer();

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnCreatePrivateRoom();

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnJoinPublicMatch();

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnInviteFriend(const FString& PlayerID);

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnOpenSettings();

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnQuitGame();

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnCharacterSelect();

	UFUNCTION(BlueprintPure, Category = "MainMenu")
	bool HasSaveData() const;

	UFUNCTION(BlueprintPure, Category = "MainMenu")
	TArray<FString> GetSaveSlotDisplayNames() const;

	UFUNCTION(BlueprintPure, Category = "MainMenu")
	FString GetPlayerDisplayName() const;

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void RefreshUI();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "MainMenu")
	EGZMainMenuTab ActiveTab = EGZMainMenuTab::Continue;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu")
	bool bHasContinueSave = false;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu")
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu")
	int32 PlayerLevel = 1;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu")
	int32 Currency = 0;
};