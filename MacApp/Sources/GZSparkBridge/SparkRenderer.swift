import Foundation

#if os(macOS)
import Metal
import MetalKit

/// Spark 2.0 渲染器协议
public protocol SparkRendererProtocol: AnyObject {
    /// 初始化渲染器，传入 Metal 设备与共享纹理描述
    func initialize(device: MTLDevice, width: Int, height: Int)

    /// 更新视锥体，触发可见区域加载/卸载
    func updateViewFrustum(position: SIMD3<Float>, direction: SIMD3<Float>, fov: Float)

    /// 设置环境变量，驱动材质 Shader
    func setEnvironment(humidity: Float, windSpeed: Float, lightIntensity: Float, rainIntensity: Float, timeOfDay: Float)

    /// 渲染一帧到目标纹理
    func render(to texture: MTLTexture, commandBuffer: MTLCommandBuffer)

    /// 当前帧率
    var currentFPS: Float { get }
}

/// 默认占位渲染器，用于接口编译与 headless 测试
@MainActor
public final class SparkRendererStub: SparkRendererProtocol {
    public private(set) var currentFPS: Float = 60.0

    public init() {}

    public func initialize(device: MTLDevice, width: Int, height: Int) {}

    public func updateViewFrustum(position: SIMD3<Float>, direction: SIMD3<Float>, fov: Float) {}

    public func setEnvironment(humidity: Float, windSpeed: Float, lightIntensity: Float, rainIntensity: Float, timeOfDay: Float) {}

    public func render(to texture: MTLTexture, commandBuffer: MTLCommandBuffer) {
        // Headless 空实现：清空为深色
    }
}

#else
/// 非 macOS 平台空类型
public final class SparkRendererStub {
    public init() {}
}
#endif
