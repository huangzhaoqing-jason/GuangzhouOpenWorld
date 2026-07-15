#if os(macOS)
import SwiftUI
import Metal
import MetalKit

/// SwiftUI 游戏渲染视图：将 Spark 2.0 输出显示为 Metal 纹理
public struct SparkRenderView: NSViewRepresentable {
    public init() {}

    public func makeNSView(context: Context) -> MTKView {
        let device = MTLCreateSystemDefaultDevice()
        let mtkView = MTKView(frame: .zero, device: device)
        mtkView.delegate = context.coordinator
        mtkView.preferredFramesPerSecond = 60
        mtkView.colorPixelFormat = .bgra8Unorm
        mtkView.enableSetNeedsDisplay = false
        mtkView.isPaused = false
        return mtkView
    }

    public func updateNSView(_ nsView: MTKView, context: Context) {}

    public func makeCoordinator() -> Coordinator {
        Coordinator()
    }

    public class Coordinator: NSObject, MTKViewDelegate {
        private let renderer = SparkRendererStub()
        private var commandQueue: MTLCommandQueue?

        override init() {
            super.init()
            if let device = MTLCreateSystemDefaultDevice() {
                commandQueue = device.makeCommandQueue()
                renderer.initialize(device: device, width: 1920, height: 1080)
                MetalTextureBridge.shared.initialize(width: 1920, height: 1080)
            }
        }

        public func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
            if let device = view.device {
                renderer.initialize(device: device, width: Int(size.width), height: Int(size.height))
                MetalTextureBridge.shared.initialize(width: Int(size.width), height: Int(size.height))
            }
        }

        public func draw(in view: MTKView) {
            guard let drawable = view.currentDrawable,
                  let commandBuffer = commandQueue?.makeCommandBuffer(),
                  let device = view.device else { return }

            if let sharedTexture = MetalTextureBridge.shared.sharedMetalTexture() {
                renderer.render(to: sharedTexture, commandBuffer: commandBuffer)
            }

            renderer.render(to: drawable.texture, commandBuffer: commandBuffer)
            commandBuffer.present(drawable)
            commandBuffer.commit()
        }
    }
}

#else
/// 非 macOS 平台空视图
public struct SparkRenderView {
    public init() {}
}
#endif
