#if os(macOS)
import SwiftUI

// MARK: - 子任务2：游戏主UI框架 - 原生容器架构

/// Mac游戏主界面：使用 NavigationSplitView 作为根容器
/// macOS 27 Beta 3 适配：uniform toolbars、edge-to-edge sidebars、统一圆角、scroll edge effect
struct GameMainUI: View {
    @EnvironmentObject var appState: AppState
    @State private var selectedSidebarItem: SidebarItem = .world

    var body: some View {
        NavigationSplitView {
            SidebarView(state: appState.uiState, selection: $selectedSidebarItem)
                .frame(minWidth: LiquidGlassConstants.sidebarMinWidth)
                .navigationSplitViewColumnWidth(
                    min: LiquidGlassConstants.sidebarMinWidth,
                    ideal: LiquidGlassConstants.sidebarIdealWidth,
                    max: LiquidGlassConstants.sidebarMaxWidth
                )
        } detail: {
            GameMainDetailView(state: appState.uiState, selectedItem: selectedSidebarItem)
        }
        // macOS 27: 安全区域融合，将HUD嵌入系统安全区域
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
    @Bindable var state: GZUIState
    @Binding var selection: SidebarItem

    var body: some View {
        List(SidebarItem.allCases, selection: $selection) { item in
            Label(item.rawValue, systemImage: item.systemImage)
                .tag(item)
                .font(.body.weight(.medium))
                // macOS 27 Golden Gate Beta 3: 侧边栏彩色图标回归
                .foregroundStyle(item.accentColor)
                .imageScale(.large)
        }
        // macOS 27: 侧边栏贴边，不再悬浮
        .listStyle(.sidebar)
        .scrollDisabled(true)
        .navigationTitle("Open广州")
        .toolbar {
            ToolbarItem {
                Button(action: { state.isSidebarVisible.toggle() }) {
                    Image(systemName: "sidebar.left")
                }
                .help("切换边栏")
                .buttonStyle(.glass)
            }
        }
    }
}

extension SidebarItem {
    var accentColor: Color {
        switch self {
        case .world: return .blue
        case .mission: return .orange
        case .vehicle: return .green
        case .codex: return .purple
        case .settings: return .gray
        }
    }
}

struct GameMainDetailView: View {
    @Bindable var state: GZUIState
    let selectedItem: SidebarItem

    var body: some View {
        ZStack {
            SparkRenderView()
                .ignoresSafeArea()

            ScrollView {
                VStack(spacing: 24) {
                    Text(selectedItem.rawValue)
                        .font(.largeTitle.weight(.bold))
                        .foregroundStyle(.primary)
                        .padding(.horizontal, 24)
                        .padding(.vertical, 16)
                        .background(
                            LiquidGlassCard()
                        )

                    Spacer()
                }
                .frame(maxWidth: .infinity, minHeight: 800)
                .padding()
            }
            // macOS 27 Golden Gate Beta 3: 统一工具栏滚动边缘效果，提升可读性
            .scrollEdgeEffectStyle(.automatic, for: .top)
        }
        // macOS 27: uniform toolbar 使用 ToolbarSpacer 与 overflow 支持
        .toolbar {
            ToolbarItem(placement: .primaryAction) {
                Button(action: {}) {
                    Image(systemName: "magnifyingglass")
                }
                .help("搜索")
                .buttonStyle(.glass)
            }

            ToolbarItem(placement: .automatic) {
                Button(action: {}) {
                    Image(systemName: "bell")
                }
                .help("通知")
                .buttonStyle(.glass)
            }

            ToolbarSpacer()

            ToolbarOverflowMenu {
                Button("导出截图", action: {})
                Button("分享位置", action: {})
            }
        }
    }
}

#else

public struct GameMainUIStub {
    public init() {}
}

#endif
