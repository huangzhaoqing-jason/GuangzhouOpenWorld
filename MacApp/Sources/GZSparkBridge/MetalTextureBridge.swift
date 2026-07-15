import Foundation

#if os(macOS)
import Metal
import MetalKit
import IOSurface
import CoreVideo

/// Metal 共享纹理桥接：SwiftUI APP 与 UE5/Spark 共享渲染纹理
/// 使用 IOSurface 实现跨进程/跨上下文零拷贝纹理共享
@MainActor
public final class MetalTextureBridge: ObservableObject {
    public static let shared = MetalTextureBridge()

    @Published public private(set) var descriptor = SharedTextureDescriptor()
    @Published public private(set) var isReady = false

    private var device: MTLDevice?
    private var sharedTexture: MTLTexture?
    private var ioSurface: IOSurface?

    public init() {}

    /// 初始化 Metal 设备并创建共享纹理
    public func initialize(width: Int = 1920, height: Int = 1080) {
        guard let device = MTLCreateSystemDefaultDevice() else {
            descriptor.bIsValid = false
            return
        }
        self.device = device
        createSharedTexture(width: width, height: height)
    }

    private func createSharedTexture(width: Int, height: Int) {
        let properties: [String: Any] = [
            kIOSurfaceWidth as String: width,
            kIOSurfaceHeight as String: height,
            kIOSurfaceBytesPerElement as String: 4,
            kIOSurfacePixelFormat as String: kCVPixelFormatType_32BGRA
        ]

        guard let surface = IOSurface(properties: properties) else {
            descriptor.bIsValid = false
            return
        }

        ioSurface = surface
        let texDescriptor = MTLTextureDescriptor()
        texDescriptor.width = width
        texDescriptor.height = height
        texDescriptor.pixelFormat = .bgra8Unorm
        texDescriptor.usage = [.renderTarget, .shaderRead]

        guard let texture = device?.makeTexture(
            descriptor: texDescriptor,
            iosurface: surface,
            plane: 0
        ) else {
            descriptor.bIsValid = false
            return
        }

        sharedTexture = texture
        descriptor.width = width
        descriptor.height = height
        descriptor.pixelFormat = "BGRA8Unorm"
        descriptor.sharedTextureHandle = IOSurfaceCreateMachPort(surface)
        descriptor.bIsValid = true
        isReady = true
    }

    /// 供 Spark 渲染器写入的共享纹理
    public func sharedMetalTexture() -> MTLTexture? {
        sharedTexture
    }

    /// 更新 UE5 后处理参数，确保 APP UI 与游戏画面色调一致
    public func updatePostProcess(exposure: Float, bloom: Float, colorTemp: Float) {
        descriptor.exposure = exposure
        descriptor.bloomIntensity = bloom
        descriptor.colorTemperature = colorTemp
    }
}

/// 共享纹理描述，跨语言边界序列化传递
public struct SharedTextureDescriptor: Sendable {
    public var width: Int = 1920
    public var height: Int = 1080
    public var pixelFormat: String = "BGRA8Unorm"
    public var sharedTextureHandle: UInt32 = 0
    public var bIsValid: Bool = false
    public var exposure: Float = 1.0
    public var bloomIntensity: Float = 1.0
    public var colorTemperature: Float = 6500.0

    public init() {}
}

#else

/// Linux / Headless 空实现
public final class MetalTextureBridge: @unchecked Sendable {
    public static let shared = MetalTextureBridge()
    public init() {}
    public func initialize(width: Int = 1920, height: Int = 1080) {}
}

public struct SharedTextureDescriptor: Sendable {
    public init() {}
}

#endif
