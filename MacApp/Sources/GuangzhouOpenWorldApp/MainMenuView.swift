#if os(macOS)
import SwiftUI

/// 主菜单：Liquid Glass APP 外壳入口
struct MainMenuView: View {
    @EnvironmentObject var appState: AppState

    var body: some View {
        ZStack {
            GameRenderBackground()
                .ignoresSafeArea()

            HStack(spacing: 80) {
                VStack(alignment: .leading, spacing: 16) {
                    Text("Open广州")
                        .font(.system(size: 56, weight: .semibold, design: .rounded))
                        .matchedGeometryEffect(id: "app-title", in: appState.transitionNamespace, properties: .frame, isSource: true)

                    Text("近未来广州 · 珠江新城第一幕（Mock UI）")
                        .font(.title3)
                        .foregroundStyle(.secondary)
                }
                .padding(.leading, 80)

                Spacer()

                VStack(alignment: .leading, spacing: 12) {
                    ForEach(AppState.MenuItem.allCases, id: \.self) { item in
                        LiquidGlassButton(
                            title: item.rawValue,
                            namespace: appState.transitionNamespace,
                            geometryId: "menu-\(item.rawValue)"
                        ) {
                            handleSelection(item)
                        }
                    }
                }
                .frame(width: 280)
                .padding(.trailing, 120)
            }
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
    }

    private func handleSelection(_ item: AppState.MenuItem) {
        switch item {
        case .play, .newGame:
            appState.transition(to: .playing)
        case .missionLog:
            appState.transition(to: .missionLog)
        case .settings:
            appState.transition(to: .settings)
        case .album:
            appState.transition(to: .album)
        case .codex:
            appState.transition(to: .codex)
        case .quit:
            NSApplication.shared.terminate(nil)
        }
    }
}

#else
public struct MainMenuViewStub {
    public init() {}
}
#endif
