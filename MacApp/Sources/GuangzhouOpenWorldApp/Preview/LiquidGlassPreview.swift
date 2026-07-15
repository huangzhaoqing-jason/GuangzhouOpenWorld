#if os(macOS)
import SwiftUI

// MARK: - Preview：正常模式 vs 低电量/减少透明度模式

#Preview("正常模式") {
    LiquidGlassPreviewContainer(
        lowPowerMode: false,
        reduceTransparency: false,
        increaseContrast: false,
        opacity: 0.28
    )
    .frame(width: 900, height: 600)
}

#Preview("低电量 + 减少透明度") {
    LiquidGlassPreviewContainer(
        lowPowerMode: true,
        reduceTransparency: true,
        increaseContrast: false,
        opacity: 0.45
    )
    .frame(width: 900, height: 600)
}

#Preview("增加对比度") {
    LiquidGlassPreviewContainer(
        lowPowerMode: false,
        reduceTransparency: false,
        increaseContrast: true,
        opacity: 0.5
    )
    .frame(width: 900, height: 600)
}

#Preview("全局透明度滑块 - Ultra Clear") {
    LiquidGlassPreviewContainer(
        lowPowerMode: false,
        reduceTransparency: false,
        increaseContrast: false,
        opacity: 0.0
    )
    .frame(width: 900, height: 600)
}

#Preview("全局透明度滑块 - Fully Tinted") {
    LiquidGlassPreviewContainer(
        lowPowerMode: false,
        reduceTransparency: false,
        increaseContrast: false,
        opacity: 1.0
    )
    .frame(width: 900, height: 600)
}

struct LiquidGlassPreviewContainer: View {
    let lowPowerMode: Bool
    let reduceTransparency: Bool
    let increaseContrast: Bool
    let opacity: Double

    @State private var previewState = GZUIState()
    @State private var sliderValue: Double = 0.5
    @State private var toggleValue: Bool = true

    var body: some View {
        ZStack {
            // 模拟游戏渲染背景
            LinearGradient(
                colors: [.blue.opacity(0.4), .purple.opacity(0.6), .black],
                startPoint: .topLeading,
                endPoint: .bottomTrailing
            )
            .ignoresSafeArea()

            VStack(spacing: 20) {
                // 使用环境变量强制当前预览模式
                LiquidGlassHUD(state: previewState)
                    .environment(\.accessibilityReduceTransparency, reduceTransparency)

                HStack(spacing: 16) {
                    LiquidGlassButtonSystem(title: "驾驶", systemImage: "car.fill") {}
                    LiquidGlassButtonSystem(title: "地图", systemImage: "map.fill") {}
                }

                LiquidGlassSlider(title: "引擎功率", value: $sliderValue, range: 0...1)
                    .frame(width: 300)

                LiquidGlassToggle(title: "雨刷", isOn: $toggleValue)
                    .frame(width: 300)

                Text("低电量: \(lowPowerMode ? "开" : "关") · 减少透明度: \(reduceTransparency ? "开" : "关") · 增加对比度: \(increaseContrast ? "开" : "关") · 透明度: \(Int(opacity * 100))%")
                    .font(.caption)
                    .foregroundStyle(.secondary)
            }
            .padding()
        }
        .environment(\.colorScheme, .dark)
        .environment(\.accessibilityIncreaseContrast, increaseContrast)
        .onAppear {
            previewState.lowPowerMode = lowPowerMode
            previewState.reduceTransparency = reduceTransparency
            previewState.increaseContrast = increaseContrast
            previewState.liquidGlassOpacity = opacity
        }
    }
}

#endif
