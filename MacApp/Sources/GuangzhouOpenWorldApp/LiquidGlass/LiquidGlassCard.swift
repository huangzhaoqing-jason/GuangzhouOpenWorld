#if os(macOS)
import SwiftUI

/// Liquid Glass 卡片：遵循 macOS 27 Developer Beta HIG
/// 深色模式透明度 ≤35%，浅色模式自动增强对比度
struct LiquidGlassCard: View {
    var cornerRadius: CGFloat = LiquidGlassConstants.cornerRadius
    var opacityOverride: Double?
    var strokeOverride: Double?

    @Environment(\.colorScheme) private var colorScheme
    @Environment(\.accessibilityReduceTransparency) private var reduceTransparency

    private var effectiveOpacity: Double {
        if reduceTransparency {
            return colorScheme == .dark ? 0.55 : 0.78
        }
        let base = opacityOverride ?? (colorScheme == .dark ? 0.22 : 0.58)
        return colorScheme == .dark ? min(base, LiquidGlassConstants.darkMaxOpacity) : max(base, LiquidGlassConstants.lightMinOpacity)
    }

    private var backgroundBoostOpacity: Double {
        colorScheme == .dark ? 0.06 : 0.10
    }

    private var strokeOpacity: Double {
        if reduceTransparency {
            return colorScheme == .dark ? 0.35 : 0.45
        }
        return strokeOverride ?? (colorScheme == .dark ? LiquidGlassConstants.darkStrokeOpacity : LiquidGlassConstants.lightStrokeOpacity)
    }

    private var shadowOpacity: Double {
        colorScheme == .dark ? 0.32 : 0.16
    }

    var body: some View {
        RoundedRectangle(cornerRadius: cornerRadius, style: .continuous)
            .fill(.ultraThinMaterial)
            .opacity(effectiveOpacity)
            .background(
                RoundedRectangle(cornerRadius: cornerRadius, style: .continuous)
                    .fill(Color.white.opacity(backgroundBoostOpacity))
            )
            .overlay(
                RoundedRectangle(cornerRadius: cornerRadius, style: .continuous)
                    .stroke(Color.white.opacity(strokeOpacity), lineWidth: 0.5)
            )
            .shadow(
                color: Color.black.opacity(shadowOpacity),
                radius: LiquidGlassConstants.shadowRadius,
                x: 0,
                y: LiquidGlassConstants.shadowYOffset
            )
    }
}

/// Liquid Glass 弹窗/表单容器
struct LiquidGlassSheet<Content: View>: View {
    let content: Content

    init(@ViewBuilder content: () -> Content) {
        self.content = content()
    }

    var body: some View {
        content
            .padding(32)
            .background(
                LiquidGlassCard(cornerRadius: LiquidGlassConstants.sheetCornerRadius)
            )
    }
}

#else
public struct LiquidGlassCardStub {
    public init() {}
}
#endif
