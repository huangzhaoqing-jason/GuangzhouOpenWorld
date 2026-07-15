#if os(macOS)
import SwiftUI
@main
struct GuangzhouOpenWorldApp: App {
    @StateObject private var appState = AppState()

    var body: some Scene {
        WindowGroup {
            RootView()
                .environmentObject(appState)
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
