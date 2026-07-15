#if os(macOS)
import SwiftUI

struct CodexContainerView: View {
    @EnvironmentObject var appState: AppState

    var body: some View {
        ZStack {
            GameRenderBackground()
                .ignoresSafeArea()

            CodexView()
                .frame(maxWidth: 1000, maxHeight: 800)
        }
    }
}

struct CodexView: View {
    @EnvironmentObject var appState: AppState
    @State private var selectedEntry: CodexEntry? = nil
    let entries = CodexEntry.samples

    var body: some View {
        LiquidGlassSheet {
            VStack(spacing: 0) {
                HStack {
                    Text("Codex")
                        .font(.largeTitle.weight(.semibold))
                    Spacer()
                    Button("完成") {
                        appState.backToMenu()
                    }
                    .buttonStyle(.borderedProminent)
                    .keyboardShortcut(.return, modifiers: [.command])
                }
                .padding(.bottom, 12)

                Divider()

                HStack(spacing: 0) {
                    List(entries, selection: $selectedEntry) { entry in
                        Text(entry.title)
                            .tag(entry)
                    }
                    .frame(width: 240)
                    .listStyle(.sidebar)
                    .accessibilityLabel("Codex 条目列表")

                    if let entry = selectedEntry {
                        CodexDetailView(entry: entry)
                            .transition(.move(edge: .trailing).combined(with: .opacity))
                    } else {
                        Text("选择一项以查看详情")
                            .foregroundStyle(.secondary)
                            .frame(maxWidth: .infinity, maxHeight: .infinity)
                    }
                }
            }
        }
    }
}

struct CodexEntry: Identifiable, Hashable {
    let id = UUID()
    let title: String
    let content: String

    static let samples: [CodexEntry] = [
        CodexEntry(title: "骑楼", content: "广州骑楼是岭南建筑代表，兼具遮阳避雨与商业功能。"),
        CodexEntry(title: "握手楼", content: "城中村高密度住宅，楼间距极小，形成独特光影。"),
        CodexEntry(title: " wetTileDrift", content: "雨天瓷砖路面湿滑，Spark Shader 实时模拟反射与溅水。"),
    ]
}

struct CodexDetailView: View {
    let entry: CodexEntry

    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 16) {
                Text(entry.title)
                    .font(.largeTitle.weight(.semibold))
                Text(entry.content)
                    .font(.body)
                    .lineSpacing(6)
                Spacer()
            }
            .padding(32)
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
}
#else
public struct CodexContainerViewStub {
    public init() {}
}
#endif
