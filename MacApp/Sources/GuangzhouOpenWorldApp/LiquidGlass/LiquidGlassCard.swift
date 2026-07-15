#if os(macOS)
import SwiftUI

/// Liquid Glass 卡片：遵循 macOS 27 Golden Gate Beta 3 HIG
/// 深色模式透明度 ≤35%，浅色模式自动增强对比度
/// macOS 27:  tighter corner radius + darkened edge + specular highlight
struct LiquidGlassCard: View {
    var cornerRadius: CGFloat = LiquidGlassConstants.cornerRadius
    var opacityOverride: Double?
    var strokeOverride: Double?

    @Environment(\.colorScheme) private var colorScheme
    @Environment(\.accessibilityReduceTransparency) private var reduceTransparency
    @Environment(\.accessibilityIncreaseContrast) private var increaseContrast

    private var effectiveOpacity: Double {
        if reduceTransparency {
            return colorScheme == .dark ? 0.55 : 0.78
        }
        if increaseContrast {
            let base = colorScheme == .dark ? 0.62 : 0.85
            return min(base + LiquidGlassConstants.increaseContrastFillBoost, 1.0)
        }
        let base = opacityOverride ?? (colorScheme == .dark ? 0.20 : 0.58)
        return colorScheme == .dark ? min(base, LiquidGlassConstants.darkMaxOpacity) : max(base, LiquidGlassConstants.lightMinOpacity)
    }

    private var strokeOpacity: Double {
        if reduceTransparency {
            return colorScheme == .dark ? 0.35 : 0.45
        }
        if increaseContrast {
            return colorScheme == .dark ? 0.45 : 0.55
        }
        return strokeOverride ?? (colorScheme == .dark ? LiquidGlassConstants.darkStrokeOpacity : LiquidGlassConstants.lightStrokeOpacity)
    }

    private var shadowOpacity: Double {
        colorScheme == .dark ? 0.30 : 0.16
    }

    var body: some View {
        RoundedRectangle(cornerRadius: cornerRadius, style: .continuous)
            .fill(.ultraThinMaterial)
            .opacity(effectiveOpacity)
            .overlay(
                RoundedRectangle(cornerRadius: cornerRadius, style: .continuous)
                    .stroke(Color.white.opacity(strokeOpacity), lineWidth: increaseContrast ? 1.0 : 0.5)
            )
            // macOS 27 Golden Gate Beta 3: 深色边缘
            .overlay(
                RoundedRectangle(cornerRadius: cornerRadius, style: .continuous)
                    .stroke(Color.black.opacity(LiquidGlassConstants.darkenedEdgeOpacity + (increaseContrast ? LiquidGlassConstants.increaseContrastStrokeBoost : 0)), lineWidth: increaseContrast ? 1.5 : 1.0)
                    .blendMode(.multiply)
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
