#if os(macOS)
import SwiftUI

@main
struct GuangzhouOpenWorldApp: App {
    @StateObject private var appState = AppState()
    @Environment(\.scenePhase) private var scenePhase

    var body: some Scene {
        WindowGroup {
            GameMainUI()
                .environmentObject(appState)
                .onChange(of: scenePhase) { _, newPhase in
                    // 生命周期同步：进入后台自动释放资源
                    if newPhase == .background {
                        appState.isGameVisible = false
                    } else if newPhase == .active {
                        appState.isGameVisible = true
                    }
                }
        }
        .windowStyle(.hiddenTitleBar)
        .defaultSize(width: 1920, height: 1080)
    }
}

#else
// Linux / 非 macOS 平台提供空入口，确保 headless 可编译
@main
struct GuangzhouOpenWorldAppStub {
    static func main() {
        // Headless 空实现：不启动 GUI
    }
}
#endif
