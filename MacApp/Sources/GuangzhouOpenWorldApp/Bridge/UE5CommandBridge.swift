import Foundation
import GZSparkBridge

#if os(macOS)
// SwiftUI 仅在 macOS APP 外壳中使用

/// 从 SwiftUI APP 发往 UE5 逻辑容器的命令
public enum UE5Command: Sendable {
    case startGame
    case pauseGame
    case resumeGame
    case quitToMenu
    case setGraphicsQuality(Int)
    case setMasterVolume(Float)
    case setLanguage(String)
    case setMouseSensitivity(Float)
    case teleportPlayer(x: Float, y: Float, z: Float)
    case updateSharedTexture(SharedTextureDescriptor)
    case syncPostProcess(exposure: Float, bloom: Float, colorTemp: Float)
}

/// 从 UE5 逻辑容器发往 SwiftUI APP 的状态事件
public enum UE5GameEvent: Sendable {
    case loadingProgress(Double)
    case sceneReady
    case playerPosition(x: Float, y: Float, z: Float)
    case performanceReport(fps: Float, memoryMB: Float)
    case missionUpdate(String)
    case postProcessUpdated(exposure: Float, bloom: Float, colorTemp: Float)
}

/// 命令桥接协议
public protocol UE5CommandBridgeProtocol: AnyObject {
    func send(_ command: UE5Command)
    var onEvent: ((UE5GameEvent) -> Void)? { get set }
}

/// 本地空桥接，用于编译与 headless 测试
@MainActor
public final class UE5CommandBridgeStub: UE5CommandBridgeProtocol {
    public var onEvent: ((UE5GameEvent) -> Void)?

    public init() {}

    public func send(_ command: UE5Command) {
        // Headless 测试：直接回送确认事件
        switch command {
        case .startGame:
            onEvent?(.loadingProgress(0.5))
            onEvent?(.sceneReady)
        case .setGraphicsQuality(let quality):
            onEvent?(.performanceReport(fps: 60.0, memoryMB: 1024.0))
        case .syncPostProcess(let exposure, let bloom, let colorTemp):
            MetalTextureBridge.shared.updatePostProcess(exposure: exposure, bloom: bloom, colorTemp: colorTemp)
            onEvent?(.postProcessUpdated(exposure: exposure, bloom: bloom, colorTemp: colorTemp))
        case .updateSharedTexture(let descriptor):
            MetalTextureBridge.shared.updatePostProcess(
                exposure: descriptor.exposure,
                bloom: descriptor.bloomIntensity,
                colorTemp: descriptor.colorTemperature
            )
        default:
            break
        }
    }
}

#else
public enum UE5CommandStub {
    case none
}

public enum UE5GameEventStub {
    case none
}

public final class UE5CommandBridgeStub {
    public init() {}
}
#endif
