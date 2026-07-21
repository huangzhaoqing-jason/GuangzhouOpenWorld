#if os(macOS)
import SwiftUI

/// APP 根视图：管理启动页/主菜单/游戏/系统页面全局过渡
/// 所有页面切换使用 matchGeometryEffect + spring，支持全程打断
struct RootView: View {
    @EnvironmentObject var appState: AppState

    var body: some View {
        ZStack {
            switch appState.phase {
            case .launch:
                LaunchView()
                    .transition(.opacity.combined(with: .scale(scale: 1.02)))
            case .mainMenu:
                MainMenuView()
                    .transition(.asymmetric(
                        insertion: .opacity.combined(with: .scale(scale: 0.98)),
                        removal: .opacity.combined(with: .scale(scale: 1.02))
                    ))
            case .playing:
                GameContainerView()
                    .transition(.opacity)
            case .settings:
                SettingsContainerView()
                    .transition(.move(edge: .trailing).combined(with: .opacity))
            case .album:
                AlbumContainerView()
                    .transition(.move(edge: .trailing).combined(with: .opacity))
            case .codex:
                CodexContainerView()
                    .transition(.move(edge: .trailing).combined(with: .opacity))
            case .missionLog:
                MissionLogContainerView()
                    .transition(.move(edge: .trailing).combined(with: .opacity))
            }
        }
        .animation(
            .spring(response: LiquidGlassConstants.springResponse, dampingFraction: LiquidGlassConstants.springDamping),
            value: appState.phase
        )
        .preferredColorScheme(.dark)
    }
}

#else
public struct RootViewStub {
    public init() {}
}
#endif
