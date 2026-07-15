#if os(macOS)
import SwiftUI

// MARK: - 子任务3：游戏交互控件 - 物理级反馈

/// 系统级物理按钮：spring动画 + hoverEffect + 按压反馈
struct LiquidGlassButtonSystem: View {
    let title: String
    let systemImage: String
    let action: () -> Void

    @State private var isPressed: Bool = false
    @FocusState private var isFocused: Bool

    var body: some View {
        Button(action: action) {
            Label(title, systemImage: systemImage)
                .font(.body.weight(.medium))
                .foregroundStyle(.primary)
                .frame(maxWidth: .infinity, alignment: .leading)
                .padding(.horizontal, 16)
                .padding(.vertical, 12)
                .background(
                    RoundedRectangle(cornerRadius: 14, style: .continuous)
                        .fill(.ultraThinMaterial)
                        .opacity(isFocused ? 0.45 : 0.28)
                )
                .overlay(
                    RoundedRectangle(cornerRadius: 14, style: .continuous)
                        .stroke(Color.white.opacity(isFocused ? 0.25 : 0.12), lineWidth: 0.5)
                )
                .scaleEffect(isPressed ? 0.96 : 1.0)
        }
        .buttonStyle(.plain)
        // 系统级悬停效果，自动适配 Mac 触控板
        .hoverEffect(.highlight)
        .focused($isFocused)
        .simultaneousGesture(
            DragGesture(minimumDistance: 0)
                .onChanged { _ in
                    // 系统标准 spring 动画，与原生控件同频
                    withAnimation(.spring(response: 0.12, dampingFraction: 0.8)) {
                        isPressed = true
                    }
                }
                .onEnded { _ in
                    withAnimation(.spring(response: 0.12, dampingFraction: 0.8)) {
                        isPressed = false
                    }
                }
        )
        .accessibilityLabel(title)
        .accessibilityAddTraits(.isButton)
    }
}

/// 系统级滑块：NSSlider 触感 + 实时数值反馈
struct LiquidGlassSlider: View {
    let title: String
    @Binding var value: Double
    let range: ClosedRange<Double>

    var body: some View {
        VStack(alignment: .leading, spacing: 6) {
            HStack {
                Text(title)
                    .font(.subheadline.weight(.medium))
                Spacer()
                Text(String(format: "%.0f%%", value * 100))
                    .font(.subheadline.monospacedDigit())
                    .foregroundStyle(.secondary)
                    .contentTransition(.numericText())
            }
            // NSSlider 原生触感反馈
            Slider(value: $value, in: range)
                .tint(.white)
        }
        .padding(12)
        .background(
            RoundedRectangle(cornerRadius: 14, style: .continuous)
                .fill(.ultraThinMaterial)
                .opacity(0.22)
        )
        .accessibilityElement(children: .combine)
        .accessibilityLabel(title)
        .accessibilityValue("\(Int(value * 100))%")
    }
}

/// 系统级开关：与 macOS 原生 Toggle 物理模型一致
struct LiquidGlassToggle: View {
    let title: String
    @Binding var isOn: Bool

    var body: some View {
        Toggle(title, isOn: $isOn)
            .toggleStyle(.switch)
            .padding(12)
            .background(
                RoundedRectangle(cornerRadius: 14, style: .continuous)
                    .fill(.ultraThinMaterial)
                    .opacity(0.22)
            )
            .accessibilityLabel(title)
    }
}

#else

public struct LiquidGlassButtonSystemStub {
    public init() {}
}

#endif
