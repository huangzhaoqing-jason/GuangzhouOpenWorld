// swift-tools-version:6.0
import PackageDescription

let products: [Product] = [
    .executable(
        name: "GuangzhouOpenWorldApp",
        targets: ["GuangzhouOpenWorldApp"]
    ),
    .library(
        name: "GZSparkBridge",
        targets: ["GZSparkBridge"]
    ),
]

let targets: [Target] = [
    .executableTarget(
        name: "GuangzhouOpenWorldApp",
        dependencies: ["GZSparkBridge"],
        swiftSettings: [
            .enableExperimentalFeature("StrictConcurrency")
        ]
    ),
    .target(
        name: "GZSparkBridge",
        swiftSettings: [
            .enableExperimentalFeature("StrictConcurrency")
        ]
    ),
    .testTarget(
        name: "GuangzhouOpenWorldAppTests",
        dependencies: ["GuangzhouOpenWorldApp", "GZSparkBridge"]
    ),
]

#if os(macOS)
// macOS 27 Developer Beta：真实目标平台
let package = Package(
    name: "GuangzhouOpenWorldApp",
    platforms: [.macOS(.v27)],
    products: products,
    dependencies: [],
    targets: targets
)
#else
// Linux Headless：不提供 macOS 平台约束，所有 Apple API 被 #if os(macOS) 隔离
let package = Package(
    name: "GuangzhouOpenWorldApp",
    products: products,
    dependencies: [],
    targets: targets
)
#endif
