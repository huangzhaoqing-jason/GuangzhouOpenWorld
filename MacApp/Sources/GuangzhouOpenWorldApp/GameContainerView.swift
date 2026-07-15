#if os(macOS)
import SwiftUI

/// 游戏容器：显示 Spark 2.0 渲染画面
/// 游戏内 HUD 不纳入 Liquid Glass，使用系统原生 overlay 以保持玩法聚焦
struct GameContainerView: View {
    @EnvironmentObject var appState: AppState

    var body: some View {
        ZStack {
            SparkRenderView()
                .ignoresSafeArea()

            VStack {
                HStack {
                    Button(action: { appState.backToMenu() }) {
                        Label("返回", systemImage: "chevron.left")
                    }
                    .buttonStyle(.bordered)
                    .padding()
                    .keyboardShortcut(.escape, modifiers: [])

                    Spacer()
                }

                Spacer()
            }
        }
    }
}

#else
public struct GameContainerViewStub {
    public init() {}
}
#endif
