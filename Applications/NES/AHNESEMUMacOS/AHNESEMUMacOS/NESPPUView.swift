//
//  NESPPUView.swift
//  AHNESEMUMacOS
//
//  Created by Alvin HEIB on 12/05/2026.
//

import SwiftUI
import SpriteKit
import NESEMU

struct NESPPUView: View {
    let ppu: NESPPU
    
    // Create the scene instance
    var scene: SKScene {
        let scene = NESRenderScene(ppu: ppu)
        scene.scaleMode = .aspectFit
        return scene
    }

    var body: some View {
        SpriteView(scene: scene)
            .edgesIgnoringSafeArea(.all)
    }
}
