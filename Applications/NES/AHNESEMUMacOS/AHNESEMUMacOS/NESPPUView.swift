import SwiftUI
import SpriteKit
import NESEMU

struct NESPPUView: View {
    @StateObject private var scene: NESRenderScene

    init(emu: NESEMU) {
        _scene = StateObject(wrappedValue: NESRenderScene(ppu: emu.ppu))
    }

    var body: some View {
        ZStack(alignment: .topTrailing) {
            SpriteView(scene: scene)
                .edgesIgnoringSafeArea(.all)

            VStack(alignment: .trailing, spacing: 4) {
                Text("FPS: \(scene.fps, specifier: "%.0f")")
                    .font(.caption)
                    .monospacedDigit()
                    .foregroundColor(.white)
            }
            .padding(.top, 8)
            .padding(.trailing, 8)
        }
        .onAppear { scene.startFPSTimer() }
        .onDisappear { scene.stopFPSTimer() }
    }
}
