#if os(macOS)
import SwiftUI

struct SettingsContainerView: View {
    @EnvironmentObject var appState: AppState

    var body: some View {
        ZStack {
            GameRenderBackground()
                .ignoresSafeArea()

            SettingsView()
                .frame(maxWidth: 900, maxHeight: 700)
        }
    }
}

struct SettingsView: View {
    @EnvironmentObject var appState: AppState
    @State private var graphicsQuality: Double = 1.0
    @State private var masterVolume: Double = 0.8
    @State private var mouseSensitivity: Double = 0.5
    @State private var selectedLanguage = "简体中文"

    var body: some View {
        LiquidGlassSheet {
            VStack(alignment: .leading, spacing: 24) {
                HStack {
                    Text("设置")
                        .font(.largeTitle.weight(.semibold))
                    Spacer()
                    Button("完成") {
                        appState.backToMenu()
                    }
                    .buttonStyle(.borderedProminent)
                    .keyboardShortcut(.return, modifiers: [.command])
                }

                Divider()

                ScrollView {
                    VStack(alignment: .leading, spacing: 20) {
                        SettingsSlider(title: "画质等级", value: $graphicsQuality, range: 0...2)
                        SettingsSlider(title: "主音量", value: $masterVolume, range: 0...1)
                        SettingsSlider(title: "鼠标灵敏度", value: $mouseSensitivity, range: 0...1)

                        Picker("语言", selection: $selectedLanguage) {
                            Text("简体中文").tag("简体中文")
                            Text("English").tag("English")
                        }
                        .pickerStyle(.segmented)
                        .accessibilityLabel("语言")

                        Toggle("显示 FPS", isOn: .constant(true))
                            .accessibilityLabel("显示每秒帧数")
                        Toggle("垂直同步", isOn: .constant(true))
                            .accessibilityLabel("垂直同步")
                        Toggle("动态分辨率", isOn: .constant(true))
                            .accessibilityLabel("动态分辨率")
                    }
                }

                Spacer()
            }
        }
    }
}

struct SettingsSlider: View {
    let title: String
    @Binding var value: Double
    let range: ClosedRange<Double>

    var body: some View {
        VStack(alignment: .leading, spacing: 6) {
            Text(title)
                .font(.headline)
            Slider(value: $value, in: range)
                .accessibilityLabel(title)
        }
    }
}

#else
public struct SettingsContainerViewStub {
    public init() {}
}
#endif
