import SwiftUI
import SpriteKit
import Combine

struct NESEMUView: View {
    @StateObject private var holder = NESEMUHolder()

    var body: some View {
        VStack {
            NESPPUView(emu: holder.emu)
                .aspectRatio(CGFloat(256 / 240), contentMode: .fit)
                .background(Color.black)

            HStack {
                Text("NES Emulator Running")
                    .font(.caption)
                Spacer()
                Text("UPS: \(holder.ups, specifier: "%.0f")")
                    .font(.caption)
                    .monospacedDigit()
            }
            .padding(.horizontal, 8)
        }
        .onAppear { holder.start() }
        .onDisappear { holder.stop() }
    }
}

