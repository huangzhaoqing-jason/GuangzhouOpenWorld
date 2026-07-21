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
    let id: String
    let title: String
    let content: String

    static let samples: [CodexEntry] = [
        CodexEntry(
            id: "world",
            title: "世界观",
            content: "近未来广州：珠江新城玻璃幕墙吞吐资本与数据；江对岸塔影下，灰产与秩序外包争夺中轴线。本作原创，气质上借鉴自由都市与霓虹赛博氛围，无关任何商业游戏 IP 角色。"
        ),
        CodexEntry(
            id: "dual",
            title: "双身份",
            content: "「顺路」跑腿探查线与「底噪」街头生存线在幕 1 汇合。对应工程里双角色系统草稿，UI 仅为说明。"
        ),
        CodexEntry(
            id: "factions",
            title: "势力",
            content: "流明控股（企业）、潮汐网格（灰产物流）、秩序租赁（外包安保/通缉）、信号掮客（任务板与情报）。详见 Docs/STORY_BIBLE.md。"
        ),
        CodexEntry(
            id: "cbd",
            title: "珠江新城",
            content: "幕 1 / M3 切片：北黄埔大道、南珠江、西广州大道、东快速路走廊。地标含花城广场、大剧院、省博、图书馆、东塔西塔意向、猎德大桥；对岸广州塔为远景锚。"
        ),
        CodexEntry(
            id: "wanted",
            title: "通缉",
            content: "0 星无压力；1–2 星巡逻压力；3+ 星封路与夜探照。Wanted 类任务用于降温。HUD 星级为 Mock。"
        ),
        CodexEntry(
            id: "rain_optics",
            title: "雨夜光学",
            content: "项目自有路面湿滑与玻璃折射设定，服务霓虹夜场景；具体着色器在 UE/Metal 侧，Codex 只作叙事提示。"
        )
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
