#if os(macOS)
import SwiftUI

/// macOS 27 Developer Beta Liquid Glass 设计规范常量
enum LiquidGlassConstants {
    /// 深色模式最大不透明度 ≤35%
    static let darkMaxOpacity: Double = 0.35
    /// 浅色模式最小不透明度，确保可读性
    static let lightMinOpacity: Double = 0.55
    /// 浅色模式增强对比度后的描边不透明度
    static let lightStrokeOpacity: Double = 0.22
    /// 深色模式描边不透明度
    static let darkStrokeOpacity: Double = 0.15
    /// 默认圆角
    static let cornerRadius: CGFloat = 20
    /// 大卡片圆角
    static let sheetCornerRadius: CGFloat = 28
    /// 默认阴影
    static let shadowRadius: CGFloat = 28
    static let shadowYOffset: CGFloat = 14
    static let shadowOpacity: Double = 0.28
    /// 苹果官方推荐动效曲线参数
    static let springResponse: Double = 0.35
    static let springDamping: Double = 0.82
    /// UI响应延迟上限 60ms
    static let maxResponseMillis: Double = 60
    /// 全局过渡时长
    static let transitionDuration: Double = 0.45
}

/// 颜色方案感知材质参数
struct LiquidGlassStyle {
    @Environment(\.colorScheme) var colorScheme

    var fillOpacity: Double {
        colorScheme == .dark ? 0.22 : 0.62
    }

    var backgroundBoostOpacity: Double {
        colorScheme == .dark ? 0.06 : 0.12
    }

    var strokeOpacity: Double {
        colorScheme == .dark ? LiquidGlassConstants.darkStrokeOpacity : LiquidGlassConstants.lightStrokeOpacity
    }

    var shadowOpacity: Double {
        colorScheme == .dark ? 0.32 : 0.18
    }
}

#endif
