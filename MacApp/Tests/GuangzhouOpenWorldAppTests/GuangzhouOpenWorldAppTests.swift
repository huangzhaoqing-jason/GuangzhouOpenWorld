import XCTest
@testable import GZSparkBridge
@testable import GuangzhouOpenWorldApp

final class GuangzhouOpenWorldAppTests: XCTestCase {
    func testSparkRendererStubExists() {
        let renderer = SparkRendererStub()
        XCTAssertNotNil(renderer)
    }

    func testAppStateStubExists() {
        let state = AppStateStub()
        XCTAssertNotNil(state)
    }

    func testMetalTextureBridgeStubExists() {
        let bridge = MetalTextureBridge.shared
        XCTAssertNotNil(bridge)
    }

    func testMissionAct1CatalogLoads() {
        let catalog = MissionCatalogLoader.loadAct1()
        XCTAssertEqual(catalog.act, 1)
        XCTAssertFalse(catalog.missions.isEmpty)
        XCTAssertEqual(catalog.district_id, "zhujiang_new_town")
    }
}
