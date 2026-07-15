#if os(macOS)
import SwiftUI

// MARK: - 子任务2：游戏主UI框架 - 原生容器架构

/// Mac游戏主界面：使用 NavigationSplitView 作为根容器，继承系统自动布局优化
struct GameMainUI: View {
    @EnvironmentObject var appState: AppState
    @State private var selectedSidebarItem: SidebarItem = .world

    var body: some View {
        NavigationSplitView {
            SidebarView(state: appState.uiState, selection: $selectedSidebarItem)
                .frame(minWidth: 220)
                .navigationSplitViewColumnWidth(min: 200, ideal: 240, max: 300)
        } detail: {
            GameMainDetailView(state: appState.uiState, selectedItem: selectedSidebarItem)
        }
        // 安全区域融合：将HUD嵌入系统安全区域
        .safeAreaInset(edge: .top, spacing: 0) {
            if appState.uiState.isHUDVisible {
                LiquidGlassHUD(state: appState.uiState)
                    .padding(.horizontal, 16)
                    .padding(.top, 8)
                    .frame(maxWidth: 520)
                    .frame(maxWidth: .infinity, alignment: .topTrailing)
            }
        }
        .environment(appState.uiState)
        .onChange(of: appState.uiState.scenePhase) { _, newPhase in
            handleScenePhase(newPhase)
        }
    }

    private func handleScenePhase(_ phase: ScenePhase) {
        switch phase {
        case .background:
            // 进入后台自动释放资源
            appState.uiState.isHUDVisible = false
        case .active:
            appState.uiState.isHUDVisible = true
        default:
            break
        }
    }
}

enum SidebarItem: String, CaseIterable, Identifiable {
    case world = "世界"
    case mission = "任务"
    case vehicle = "载具"
    case codex = "Codex"
    case settings = "设置"

    var id: String { rawValue }

    var systemImage: String {
        switch self {
        case .world: return "globe.asia.australia.fill"
        case .mission: return "list.bullet.rectangle.fill"
        case .vehicle: return "car.fill"
        case .codex: return "books.vertical.fill"
        case .settings: return "gearshape.fill"
        }
    }
}

struct SidebarView: View {
    let state: GZUIState
    @Binding var selection: SidebarItem

    var body: some View {
        List(SidebarItem.allCases, selection: $selection) { item in
            Label(item.rawValue, systemImage: item.systemImage)
                .tag(item)
        }
        .listStyle(.sidebar)
        .navigationTitle("Open广州")
        .toolbar {
            ToolbarItem {
                Button(action: { state.isSidebarVisible.toggle() }) {
                    Image(systemName: "sidebar.left")
                }
                .help("切换边栏")
            }
        }
    }
}

struct GameMainDetailView: View {
    let state: GZUIState
    let selectedItem: SidebarItem

    var body: some View {
        ZStack {
            SparkRenderView()
                .ignoresSafeArea()

            VStack {
                Text(selectedItem.rawValue)
                    .font(.largeTitle.weight(.bold))
                    .foregroundStyle(.primary)
                    .padding()
                    .background(
                        RoundedRectangle(cornerRadius: 16, style: .continuous)
                            .fill(.ultraThinMaterial)
                    )

                Spacer()
            }
            .padding()
        }
    }
}

#else

public struct GameMainUIStub {
    public init() {}
}

#endif
