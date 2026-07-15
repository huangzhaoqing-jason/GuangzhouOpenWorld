#if os(macOS)
import SwiftUI

/// 启动页：展示 Spark 实时渲染背景与加载进度
struct LaunchView: View {
    @EnvironmentObject var appState: AppState
    @State private var progress: Double = 0.0
    @State private var titleOpacity: Double = 0.0

    var body: some View {
        ZStack {
            GameRenderBackground()
                .ignoresSafeArea()

            VStack(spacing: 24) {
                Spacer()

                Text("Open广州")
                    .font(.system(size: 64, weight: .semibold, design: .rounded))
                    .foregroundStyle(.primary)
                    .opacity(titleOpacity)
                    .matchedGeometryEffect(id: "app-title", in: appState.transitionNamespace, properties: .frame, isSource: true)

                Text("广州环境模拟 · 4A级开放世界")
                    .font(.title3)
                    .foregroundStyle(.secondary)
                    .opacity(titleOpacity)

                Spacer()

                ProgressView(value: progress, total: 1.0)
                    .progressViewStyle(.circular)
                    .scaleEffect(1.2)
                    .padding(.bottom, 48)
            }
            .padding(48)
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .onAppear {
            withAnimation(.easeInOut(duration: 0.8)) {
                titleOpacity = 1.0
            }
            startLoading()
        }
    }

    private func startLoading() {
        Task {
            for await _ in Timer.publish(every: 0.05, on: .main, in: .common).autoconnect().values {
                progress += 0.02
                if progress >= 1.0 {
                    appState.transition(to: .mainMenu)
                    break
                }
            }
        }
    }
}

struct GameRenderBackground: View {
    var body: some View {
        Color.black
            .overlay(
                LinearGradient(
                    colors: [
                        Color.black.opacity(0.7),
                        Color.blue.opacity(0.15),
                        Color.black.opacity(0.9)
                    ],
                    startPoint: .topLeading,
                    endPoint: .bottomTrailing
                )
            )
    }
}
#else
public struct LaunchViewStub {
    public init() {}
}
#endif
