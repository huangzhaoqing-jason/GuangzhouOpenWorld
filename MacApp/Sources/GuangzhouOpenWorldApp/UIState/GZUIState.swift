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

    /// 系统级联动：增加对比度辅助功能
    public var increaseContrast: Bool = false

    /// 系统级联动：低电量模式
    public var lowPowerMode: Bool = false

    /// macOS 27 Beta 3: 全局 Liquid Glass 透明度滑块（0=Ultra Clear, 1=Fully Tinted）
    public var liquidGlassOpacity: Double = 0.5

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

    /// 启动系统设置监听：低电量、辅助功能、全局 Liquid Glass 透明度
    public func startSystemObservation() {
        syncSystemState()

        // macOS 27 Golden Gate Beta 3: 通过 NSNotificationCenter 监听全局 Liquid Glass 透明度滑块
        // 公开 API 确认后，可替换为官方环境值或通知名
        NotificationCenter.default.addObserver(
            forName: NSNotification.Name("AppleLiquidGlassOpacityDidChangeNotification"),
            object: nil,
            queue: .main
        ) { [weak self] note in
            if let opacity = note.userInfo?["opacity"] as? Double {
                self?.liquidGlassOpacity = opacity
            }
        }

        // 低电量模式变化监听
        NotificationCenter.default.addObserver(
            forName: NSNotification.Name.NSProcessInfoPowerStateDidChange,
            object: nil,
            queue: .main
        ) { [weak self] _ in
            self?.lowPowerMode = ProcessInfo.processInfo.isLowPowerModeEnabled
        }

        // 辅助功能变化监听（减少透明度 / 增加对比度）
        NotificationCenter.default.addObserver(
            forName: NSNotification.Name("NSWorkspaceAccessibilityDisplayOptionsDidChangeNotification"),
            object: nil,
            queue: .main
        ) { [weak self] _ in
            self?.syncSystemState()
        }
    }

    private func syncSystemState() {
        self.lowPowerMode = ProcessInfo.processInfo.isLowPowerModeEnabled
        self.reduceTransparency = NSWorkspace.shared.accessibilityDisplayShouldReduceTransparency
        self.increaseContrast = NSWorkspace.shared.accessibilityDisplayShouldIncreaseContrast
    }

    /// 计算当前玻璃材质有效透明度（融合全局滑块与辅助功能）
    public var effectiveGlassOpacity: Double {
        if reduceTransparency {
            return colorSchemeAdjustedOpacity(base: 0.55, light: 0.78)
        }
        if increaseContrast {
            return colorSchemeAdjustedOpacity(base: 0.62, light: 0.85)
        }
        // 滑块 0..1 映射到 0.12..0.65（留足 Ultra Clear 与 Fully Tinted 区间）
        let mapped = 0.12 + (liquidGlassOpacity * 0.53)
        let clamped = min(max(mapped, 0.0), LiquidGlassConstants.darkMaxOpacity)
        return colorSchemeAdjustedOpacity(base: clamped, light: LiquidGlassConstants.lightMinOpacity)
    }

    /// 计算当前描边有效不透明度
    public var effectiveStrokeOpacity: Double {
        if reduceTransparency {
            return colorSchemeAdjustedOpacity(base: 0.35, light: 0.45)
        }
        if increaseContrast {
            return colorSchemeAdjustedOpacity(base: 0.45, light: 0.55)
        }
        return colorSchemeAdjustedOpacity(base: LiquidGlassConstants.darkStrokeOpacity, light: LiquidGlassConstants.lightStrokeOpacity)
    }

    /// 计算当前阴影有效不透明度（低电量模式自动降低）
    public var effectiveShadowOpacity: Double {
        let base = colorScheme == .dark ? 0.30 : 0.16
        if lowPowerMode { return base * 0.5 }
        return base
    }

    private var colorScheme: ColorScheme {
        // 全局状态默认跟随系统；HUD 强制深色可在视图层覆盖
        .dark
    }

    private func colorSchemeAdjustedOpacity(base dark: Double, light: Double) -> Double {
        colorScheme == .dark ? dark : light
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
