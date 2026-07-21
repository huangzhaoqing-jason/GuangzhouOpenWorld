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
    case startMission(String)
    case completeMission(String)
    case setWantedStars(Int)
}

/// 从 UE5 逻辑容器发往 SwiftUI APP 的状态事件
public enum UE5GameEvent: Sendable {
    case loadingProgress(Double)
    case sceneReady
    case playerPosition(x: Float, y: Float, z: Float)
    case performanceReport(fps: Float, memoryMB: Float)
    case missionUpdate(String)
    case wantedChanged(Int)
    case postProcessUpdated(exposure: Float, bloom: Float, colorTemp: Float)
}

/// 命令桥接协议
public protocol UE5CommandBridgeProtocol: AnyObject {
    func send(_ command: UE5Command)
    var onEvent: ((UE5GameEvent) -> Void)? { get set }
}

/// 本地空桥接，用于编译与 headless 测试；mission/wanted 本地回放
@MainActor
public final class UE5CommandBridgeStub: UE5CommandBridgeProtocol {
    public var onEvent: ((UE5GameEvent) -> Void)?

    public init() {}

    public func send(_ command: UE5Command) {
        switch command {
        case .startGame:
            onEvent?(.loadingProgress(0.5))
            onEvent?(.sceneReady)
            onEvent?(.missionUpdate("夜配异常单｜在花城广场接单终端确认订单"))
            onEvent?(.wantedChanged(0))
        case .setGraphicsQuality:
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
        case .startMission(let missionId):
            onEvent?(.missionUpdate("START \(missionId)"))
        case .completeMission(let missionId):
            onEvent?(.missionUpdate("COMPLETE \(missionId)"))
        case .setWantedStars(let stars):
            onEvent?(.wantedChanged(min(max(stars, 0), 5)))
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
