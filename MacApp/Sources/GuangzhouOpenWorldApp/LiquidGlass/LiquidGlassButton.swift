#if os(macOS)
import SwiftUI

/// Liquid Glass 按钮：支持 matchGeometryEffect、弹簧动画、全程打断、键盘导航
struct LiquidGlassButton: View {
    let title: String
    var namespace: Namespace.ID?
    var geometryId: String?
    let action: () -> Void

    @State private var isHovered: Bool = false
    @State private var isPressed: Bool = false
    @FocusState private var isFocused: Bool

    var body: some View {
        Button(action: action) {
            Text(title)
                .font(.title3.weight(.medium))
                .dynamicTypeSize(.small ... .accessibility3)
                .foregroundStyle(.primary)
                .frame(maxWidth: .infinity, alignment: .leading)
                .padding(.horizontal, 20)
                .padding(.vertical, 14)
                .background(
                    LiquidGlassCard()
                        .applyGeometry(namespace: namespace, id: geometryId)
                )
                .scaleEffect(isPressed ? 0.96 : (isHovered ? 1.02 : 1.0))
        }
        .buttonStyle(.plain)
        .focused($isFocused)
        .onHover { hovering in
            withAnimation(.spring(response: 0.18, dampingFraction: 0.75)) {
                isHovered = hovering
            }
        }
        .simultaneousGesture(
            DragGesture(minimumDistance: 0)
                .onChanged { _ in
                    withAnimation(.spring(response: 0.12, dampingFraction: 0.8)) {
                        isPressed = true
                    }
                }
                .onEnded { _ in
                    withAnimation(.spring(response: 0.12, dampingFraction: 0.8)) {
                        isPressed = false
                    }
                }
        )
        .accessibilityElement(children: .ignore)
        .accessibilityLabel(title)
        .accessibilityHint("按回车或空格激活")
        .accessibilityAddTraits(.isButton)
        .accessibilityAddTraits(isFocused ? .isSelected : [])
    }
}

private extension View {
    @ViewBuilder
    func applyGeometry(namespace: Namespace.ID?, id: String?) -> some View {
        if let namespace = namespace, let id = id {
            self.matchedGeometryEffect(id: id, in: namespace, properties: .frame, isSource: true)
        } else {
            self
        }
    }
}

#else
public struct LiquidGlassButtonStub {
    public init() {}
}
#endif
