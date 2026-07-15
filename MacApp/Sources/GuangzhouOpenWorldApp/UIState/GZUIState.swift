#if os(macOS)
import SwiftUI
import Combine

// MARK: - Swift 6.4 并发安全全局状态

/// 游戏UI全局状态，使用 @Observable 实现差异渲染
@MainActor
@Observable
public final class GZUIState {
    /// 当前场景阶段，与系统 ScenePhase 同步
    public var scenePhase: ScenePhase = .inactive

    /// 当前顶层导航
    public var selectedHUDTab: HUDTab = .drive

    /// 系统级联动：减少透明度辅助功能
    public var reduceTransparency: Bool = false

    /// 系统级联动：低电量模式
    public var lowPowerMode: Bool = false

    /// 用户可调全局玻璃透明度（模拟/联动系统设置）
    public var liquidGlassOpacity: Double = 0.28

    /// 实时性能数据
    public var fps: Double = 60.0
    public var memoryMB: Double = 1024.0

    /// HUD可见性
    public var isHUDVisible: Bool = true

    /// 导航栏展开状态
    public var isSidebarVisible: Bool = true

    public init() {}

    /// sending 关键字确保跨 actor 传递数据无竞争
    public func updatePerformance(_ report: sending GZPerformanceReport) {
        self.fps = report.fps
        self.memoryMB = report.memoryMB
    }

    public enum HUDTab: String, CaseIterable, Identifiable {
        case drive = "驾驶"
        case map = "地图"
        case mission = "任务"
        case inventory = "物品"

        public var id: String { rawValue }

        public var systemImage: String {
            switch self {
            case .drive: return "car.fill"
            case .map: return "map.fill"
            case .mission: return "list.bullet.rectangle.fill"
            case .inventory: return "backpack.fill"
            }
        }
    }
}

/// 性能报告（Sendable + sending）
public struct GZPerformanceReport: Sendable {
    public let fps: Double
    public let memoryMB: Double

    public init(fps: Double, memoryMB: Double) {
        self.fps = fps
        self.memoryMB = memoryMB
    }
}

#else

public final class GZUIStateStub {
    public init() {}
}

public struct GZPerformanceReportStub: Sendable {
    public init() {}
}

#endif
