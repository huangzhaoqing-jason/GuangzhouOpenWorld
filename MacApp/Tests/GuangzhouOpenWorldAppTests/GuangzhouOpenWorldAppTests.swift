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
}
