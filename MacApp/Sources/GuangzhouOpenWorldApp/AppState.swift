#if os(macOS)
import SwiftUI
import Combine

@MainActor
final class AppState: ObservableObject {
    enum AppPhase: Equatable {
        case launch
        case mainMenu
        case playing
        case settings
        case album
        case codex
    }

    @Published var phase: AppPhase = .launch
    @Published var isTransitioning: Bool = false
    @Published var selectedMenuItem: MenuItem? = .play

    /// 全局命名空间，供 matchGeometryEffect 跨页面共享元素动画
    let transitionNamespace = Namespace().wrappedValue

    enum MenuItem: String, CaseIterable {
        case play = "继续游戏"
        case newGame = "新游戏"
        case settings = "设置"
        case album = "相册"
        case codex = "Codex"
        case quit = "退出"
    }

    func transition(to newPhase: AppPhase) {
        guard phase != newPhase else { return }
        isTransitioning = true
        withAnimation(.spring(response: LiquidGlassConstants.springResponse, dampingFraction: LiquidGlassConstants.springDamping)) {
            phase = newPhase
        }
        DispatchQueue.main.asyncAfter(deadline: .now() + LiquidGlassConstants.transitionDuration) {
            self.isTransitioning = false
        }
    }

    func backToMenu() {
        transition(to: .mainMenu)
    }
}

#else
public final class AppStateStub {
    public init() {}
}
#endif
