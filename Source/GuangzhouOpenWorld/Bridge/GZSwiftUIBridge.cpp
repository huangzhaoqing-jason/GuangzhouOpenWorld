#include "Bridge/GZSwiftUIBridge.h"
#include "Engine/Engine.h"

UGZSwiftUIBridge::UGZSwiftUIBridge()
{
}

void UGZSwiftUIBridge::Initialize()
{
	bInitialized = true;
}

void UGZSwiftUIBridge::ReceiveCommand(const FUE5Command& Command)
{
	if (!bInitialized)
	{
		Initialize();
	}

	DispatchCommand(Command);
	OnCommandReceived.Broadcast(Command);
}

void UGZSwiftUIBridge::SendEvent(const FString& EventName, const FString& Payload)
{
	OnEventSent.Broadcast(EventName, Payload);
}

void UGZSwiftUIBridge::DispatchCommand(const FUE5Command& Command)
{
	switch (Command.Type)
	{
	case EUE5CommandType::StartGame:
		UE_LOG(LogTemp, Log, TEXT("[SwiftUIBridge] StartGame"));
		break;
	case EUE5CommandType::PauseGame:
		UE_LOG(LogTemp, Log, TEXT("[SwiftUIBridge] PauseGame"));
		break;
	case EUE5CommandType::ResumeGame:
		UE_LOG(LogTemp, Log, TEXT("[SwiftUIBridge] ResumeGame"));
		break;
	case EUE5CommandType::QuitToMenu:
		UE_LOG(LogTemp, Log, TEXT("[SwiftUIBridge] QuitToMenu"));
		break;
	case EUE5CommandType::SetGraphicsQuality:
		UE_LOG(LogTemp, Log, TEXT("[SwiftUIBridge] SetGraphicsQuality=%d"), Command.IntValue);
		break;
	case EUE5CommandType::SetMasterVolume:
		UE_LOG(LogTemp, Log, TEXT("[SwiftUIBridge] SetMasterVolume=%.2f"), Command.FloatValue);
		break;
	case EUE5CommandType::SetLanguage:
		UE_LOG(LogTemp, Log, TEXT("[SwiftUIBridge] SetLanguage=%s"), *Command.StringValue);
		break;
	case EUE5CommandType::SetMouseSensitivity:
		UE_LOG(LogTemp, Log, TEXT("[SwiftUIBridge] SetMouseSensitivity=%.2f"), Command.FloatValue);
		break;
	case EUE5CommandType::TeleportPlayer:
		UE_LOG(LogTemp, Log, TEXT("[SwiftUIBridge] TeleportPlayer=%s"), *Command.VectorValue.ToString());
		break;
	default:
		break;
	}
}
