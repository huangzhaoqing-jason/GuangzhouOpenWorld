#if os(macOS)
import SwiftUI

/// macOS 27 Golden Gate Beta 3 Liquid Glass 设计规范常量
/// 参考公开信息：
/// - 系统新增全局 Liquid Glass 透明度滑块（Ultra Clear → Fully Tinted）
/// - 深色边缘（darkened edge）与更亮高光（brighter specular highlights）
/// - 统一窗口圆角（standardized window corner radii）
/// - 工具栏统一（uniform toolbars）与贴边侧边栏（edge-to-edge sidebars）
enum LiquidGlassConstants {
    /// 深色模式最大不透明度 ≤35%
    static let darkMaxOpacity: Double = 0.35
    /// 浅色模式最小不透明度，确保可读性
    static let lightMinOpacity: Double = 0.55
    /// 浅色模式增强对比度后的描边不透明度
    static let lightStrokeOpacity: Double = 0.22
    /// 深色模式描边不透明度
    static let darkStrokeOpacity: Double = 0.15
    /// macOS 27 统一窗口/卡片圆角（ tighter radii ）
    static let cornerRadius: CGFloat = 16
    /// 大表单/弹窗圆角
    static let sheetCornerRadius: CGFloat = 24
    /// 默认阴影
    static let shadowRadius: CGFloat = 24
    static let shadowYOffset: CGFloat = 12
    static let shadowOpacity: Double = 0.24
    /// 苹果官方推荐动效曲线参数（与系统原生弹窗一致）
    static let springResponse: Double = 0.35
    static let springDamping: Double = 0.75
    /// UI响应延迟上限 60ms（本模块目标 <8ms）
    static let maxResponseMillis: Double = 60
    /// 全局过渡时长
    static let transitionDuration: Double = 0.45
    /// macOS 27 深色边缘强度
    static let darkenedEdgeOpacity: Double = 0.18
    /// macOS 27 高光反射强度
    static let specularHighlightOpacity: Double = 0.35
    /// 增加对比度模式下的填充不透明度增强量
    static let increaseContrastFillBoost: Double = 0.12
    /// 增加对比度模式下的描边不透明度增强量
    static let increaseContrastStrokeBoost: Double = 0.10
    /// 侧边栏贴边最小宽度
    static let sidebarMinWidth: CGFloat = 200
    static let sidebarIdealWidth: CGFloat = 240
    static let sidebarMaxWidth: CGFloat = 300
    /// 侧边栏彩色图标尺寸
    static let sidebarIconSize: CGFloat = 22
    /// 工具栏统一高度参考
    static let toolbarHeight: CGFloat = 38
}

/// 颜色方案感知材质参数
struct LiquidGlassStyle {
    @Environment(\.colorScheme) var colorScheme
    @Environment(\.accessibilityReduceTransparency) var reduceTransparency

    var fillOpacity: Double {
        if reduceTransparency { return colorScheme == .dark ? 0.55 : 0.78 }
        return colorScheme == .dark ? 0.20 : 0.58
    }

    var strokeOpacity: Double {
        if reduceTransparency { return colorScheme == .dark ? 0.35 : 0.45 }
        return colorScheme == .dark ? LiquidGlassConstants.darkStrokeOpacity : LiquidGlassConstants.lightStrokeOpacity
    }

    var shadowOpacity: Double {
        colorScheme == .dark ? 0.30 : 0.16
    }
}

#else
public struct LiquidGlassConstantsStub {
    public init() {}
}
#endif
