#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZSwiftUIBridge.generated.h"

UENUM(BlueprintType)
enum class EUE5CommandType : uint8
{
	StartGame			UMETA(DisplayName = "Start Game"),
	PauseGame			UMETA(DisplayName = "Pause Game"),
	ResumeGame			UMETA(DisplayName = "Resume Game"),
	QuitToMenu			UMETA(DisplayName = "Quit To Menu"),
	SetGraphicsQuality	UMETA(DisplayName = "Set Graphics Quality"),
	SetMasterVolume		UMETA(DisplayName = "Set Master Volume"),
	SetLanguage			UMETA(DisplayName = "Set Language"),
	SetMouseSensitivity	UMETA(DisplayName = "Set Mouse Sensitivity"),
	TeleportPlayer		UMETA(DisplayName = "Teleport Player"),
};

USTRUCT(BlueprintType)
struct FUE5Command
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EUE5CommandType Type = EUE5CommandType::StartGame;

	UPROPERTY(BlueprintReadWrite)
	int32 IntValue = 0;

	UPROPERTY(BlueprintReadWrite)
	float FloatValue = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	FString StringValue;

	UPROPERTY(BlueprintReadWrite)
	FVector VectorValue = FVector::ZeroVector;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwiftUICommandReceived, const FUE5Command&, Command);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwiftUIEventSent, const FString&, EventName, const FString&, Payload);

/**
 * SwiftUI APP 与 UE5 逻辑容器之间的命令桥接。
 * 负责接收来自 macOS APP 的生命周期与设置命令，并向 APP 回送状态事件。
 */
UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZSwiftUIBridge : public UObject
{
	GENERATED_BODY()

public:
	UGZSwiftUIBridge();

	UFUNCTION(BlueprintCallable, Category = "SwiftUI Bridge")
	void Initialize();

	/** 接收来自 SwiftUI APP 的命令 */
	UFUNCTION(BlueprintCallable, Category = "SwiftUI Bridge")
	void ReceiveCommand(const FUE5Command& Command);

	/** 向 SwiftUI APP 发送事件 */
	UFUNCTION(BlueprintCallable, Category = "SwiftUI Bridge")
	void SendEvent(const FString& EventName, const FString& Payload);

	UPROPERTY(BlueprintAssignable, Category = "SwiftUI Bridge")
	FOnSwiftUICommandReceived OnCommandReceived;

	UPROPERTY(BlueprintAssignable, Category = "SwiftUI Bridge")
	FOnSwiftUIEventSent OnEventSent;

private:
	void DispatchCommand(const FUE5Command& Command);

	UPROPERTY()
	bool bInitialized = false;
};
