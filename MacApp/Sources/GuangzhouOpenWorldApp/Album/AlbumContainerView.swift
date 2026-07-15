#if os(macOS)
import SwiftUI

struct AlbumContainerView: View {
    @EnvironmentObject var appState: AppState

    var body: some View {
        ZStack {
            GameRenderBackground()
                .ignoresSafeArea()

            AlbumView()
                .frame(maxWidth: 1200, maxHeight: 800)
        }
    }
}

struct AlbumView: View {
    @EnvironmentObject var appState: AppState
    @State private var selectedItem: AlbumItem? = nil
    let items = AlbumItem.samples

    var body: some View {
        LiquidGlassSheet {
            VStack(alignment: .leading, spacing: 20) {
                HStack {
                    Text("相册")
                        .font(.largeTitle.weight(.semibold))
                    Spacer()
                    Button("完成") {
                        appState.backToMenu()
                    }
                    .buttonStyle(.borderedProminent)
                    .keyboardShortcut(.return, modifiers: [.command])
                }

                ScrollView {
                    LazyVGrid(columns: [GridItem(.adaptive(minimum: 180))], spacing: 16) {
                        ForEach(items) { item in
                            AlbumThumbnail(item: item)
                                .onTapGesture {
                                    withAnimation(.spring(response: 0.3, dampingFraction: 0.8)) {
                                        selectedItem = item
                                    }
                                }
                        }
                    }
                }

                Spacer()
            }
        }
        .sheet(item: $selectedItem) { item in
            AlbumDetailView(item: item)
        }
    }
}

struct AlbumItem: Identifiable {
    let id = UUID()
    let title: String
    let location: String

    static let samples: [AlbumItem] = [
        AlbumItem(title: "天河CBD雨夜", location: "天河"),
        AlbumItem(title: "越秀骑楼", location: "越秀"),
        AlbumItem(title: "珠江日落", location: "海珠"),
        AlbumItem(title: "握手楼晨光", location: "白云"),
    ]
}

struct AlbumThumbnail: View {
    let item: AlbumItem

    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            RoundedRectangle(cornerRadius: 12, style: .continuous)
                .fill(Color.gray.opacity(0.3))
                .aspectRatio(4 / 3, contentMode: .fit)

            Text(item.title)
                .font(.headline)
            Text(item.location)
                .font(.caption)
                .foregroundStyle(.secondary)
        }
        .padding(8)
        .background(LiquidGlassCard(cornerRadius: 14))
        .accessibilityElement(children: .combine)
        .accessibilityLabel("\(item.title)，位于\(item.location)")
        .accessibilityAddTraits(.isButton)
    }
}

struct AlbumDetailView: View {
    let item: AlbumItem

    var body: some View {
        LiquidGlassSheet {
            VStack(alignment: .leading, spacing: 16) {
                Text(item.title)
                    .font(.title)
                Text(item.location)
                    .foregroundStyle(.secondary)
            }
            .frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .topLeading)
        }
        .frame(width: 600, height: 400)
    }
}
#else
public struct AlbumContainerViewStub {
    public init() {}
}
#endif
