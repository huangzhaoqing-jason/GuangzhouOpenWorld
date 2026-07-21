#if os(macOS)
import SwiftUI

struct MissionLogContainerView: View {
    @EnvironmentObject var appState: AppState

    var body: some View {
        ZStack {
            GameRenderBackground()
                .ignoresSafeArea()

            MissionLogView()
                .frame(maxWidth: 1000, maxHeight: 800)
        }
    }
}

struct MissionLogView: View {
    @EnvironmentObject var appState: AppState
    @State private var selected: MissionSeed?
    private let catalog = MissionCatalogLoader.loadAct1()

    var body: some View {
        LiquidGlassSheet {
            VStack(spacing: 0) {
                HStack {
                    VStack(alignment: .leading, spacing: 4) {
                        Text("任务日志")
                            .font(.largeTitle.weight(.semibold))
                        Text("幕 \(catalog.act) · \(catalog.title) · \(catalog.district_id)")
                            .font(.caption)
                            .foregroundStyle(.secondary)
                    }
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
                    List(catalog.missions, selection: $selected) { mission in
                        VStack(alignment: .leading, spacing: 4) {
                            Text(mission.title)
                                .font(.body.weight(.medium))
                            Text("\(mission.type) · \(mission.status)")
                                .font(.caption)
                                .foregroundStyle(.secondary)
                        }
                        .tag(mission)
                    }
                    .frame(width: 280)
                    .listStyle(.sidebar)
                    .accessibilityLabel("任务列表")

                    if let mission = selected {
                        MissionDetailView(mission: mission)
                            .transition(.move(edge: .trailing).combined(with: .opacity))
                    } else {
                        Text("选择任务查看目标与奖励（Mock，未连 UE）")
                            .foregroundStyle(.secondary)
                            .frame(maxWidth: .infinity, maxHeight: .infinity)
                    }
                }
            }
        }
        .onAppear {
            if selected == nil {
                selected = catalog.missions.first
            }
            appState.uiState.applyMissionMock(catalog.missions.first { $0.status == "Available" } ?? catalog.missions.first)
        }
    }
}

struct MissionDetailView: View {
    let mission: MissionSeed

    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 16) {
                Text(mission.title)
                    .font(.largeTitle.weight(.semibold))
                Label(mission.type, systemImage: "flag.fill")
                    .font(.subheadline)
                    .foregroundStyle(.secondary)

                if let landmark = mission.landmark {
                    Text("地标：\(landmark)")
                        .font(.callout)
                }
                Text("区划：\(mission.district_id)")
                    .font(.callout)
                Text("状态：\(mission.status) · 通缉风险 \(mission.wanted_risk ?? 0)")
                    .font(.callout)

                if let requires = mission.requires, !requires.isEmpty {
                    Text("前置：\(requires.joined(separator: ", "))")
                        .font(.callout)
                        .foregroundStyle(.secondary)
                }

                Divider()

                Text("目标")
                    .font(.headline)
                ForEach(Array(mission.objectives.enumerated()), id: \.offset) { _, obj in
                    Text("• \(obj.description) ×\(obj.targetCount)")
                        .font(.body)
                }

                if let reward = mission.reward, let cash = reward.cash {
                    Text("奖励现金（占位）：\(cash)")
                        .font(.body)
                        .padding(.top, 8)
                }

                Spacer()
            }
            .padding(32)
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
}

#else
public struct MissionLogContainerViewStub {
    public init() {}
}
#endif
