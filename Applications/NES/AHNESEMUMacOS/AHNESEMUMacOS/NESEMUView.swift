//
//  NESEMUView.swift
//  AHNESEMUMacOS
//
//  Created by Alvin HEIB on 12/05/2026.
//

import SwiftUI
import SpriteKit
import NESEMU

struct NESEMUView: View {
    private let emu = NESEMU()
    private var ppu: NESPPU { emu.ppu }

    var body: some View {
        VStack {
            NESPPUView(ppu: ppu)
                .aspectRatio(CGFloat(ppu.width / ppu.height), contentMode: .fit)
                .background(Color.black)
            
            // Add your controller overlays or debug info here
            Text("NES Emulator Running")
                .font(.caption)
        }
    }
}
