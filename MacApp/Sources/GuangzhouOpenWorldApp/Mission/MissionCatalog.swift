import Foundation

/// 幕 1 任务种子（与 Docs/Data/missions_act1.json 同结构）
struct MissionActCatalog: Codable, Sendable {
    let version: Int
    let act: Int
    let title: String
    let district_id: String
    let notes: String?
    let missions: [MissionSeed]
}

struct MissionSeed: Codable, Identifiable, Hashable, Sendable {
    let id: String
    let type: String
    let title: String
    let district_id: String
    let landmark: String?
    let status: String
    let objectives: [MissionObjectiveSeed]
    let reward: MissionRewardSeed?
    let wanted_risk: Int?
    let requires: [String]?
}

struct MissionObjectiveSeed: Codable, Hashable, Sendable {
    let description: String
    let targetCount: Int
}

struct MissionRewardSeed: Codable, Hashable, Sendable {
    let cash: Int?
    let item: String?
    let wanted_delta: Int?
    let codex: String?
    let rep_signal_broker: Int?
    let rep_tide_grid: Int?
    let rep_lumen_hold: Int?
}

enum MissionCatalogLoader {
    static func loadAct1() -> MissionActCatalog {
        if let url = Bundle.module.url(forResource: "missions_act1", withExtension: "json"),
           let data = try? Data(contentsOf: url),
           let catalog = try? JSONDecoder().decode(MissionActCatalog.self, from: data) {
            return catalog
        }
        return .fallback
    }
}

extension MissionActCatalog {
    static let fallback = MissionActCatalog(
        version: 1,
        act: 1,
        title: "中轴线",
        district_id: "zhujiang_new_town",
        notes: "Fallback — bundle JSON missing",
        missions: [
            MissionSeed(
                id: "ms_01_night_drop",
                type: "MainStory",
                title: "夜配异常单",
                district_id: "zhujiang_new_town",
                landmark: "huacheng_square",
                status: "Available",
                objectives: [MissionObjectiveSeed(description: "在花城广场接单", targetCount: 1)],
                reward: MissionRewardSeed(cash: 800, item: nil, wanted_delta: nil, codex: nil, rep_signal_broker: 5, rep_tide_grid: nil, rep_lumen_hold: nil),
                wanted_risk: 0,
                requires: nil
            )
        ]
    )
}
