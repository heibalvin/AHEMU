//
//  NESRenderScene.swift
//  AHNESEMUMacOS
//
//  Created by Alvin HEIB on 12/05/2026.
//

import SwiftUI
import SpriteKit
import NESEMU

class NESRenderScene: SKScene {
    private let ppu: NESPPU
    private let screenNode = SKSpriteNode()
    private var lastUpdateTime: TimeInterval = 0
    
    init(ppu: NESPPU) {
        self.ppu = ppu
        super.init(size: CGSize(width: ppu.width, height: ppu.height))
        
        // Initialize the screen node
        screenNode.position = CGPoint(x: ppu.width / 2, y: ppu.height / 2) // Center of 256x240
        addChild(screenNode)
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func update(_ currentTime: TimeInterval) {
        // Calculate Delta Time
        if lastUpdateTime == 0 { lastUpdateTime = currentTime }
        let delta = currentTime - lastUpdateTime
        lastUpdateTime = currentTime
        
        // 1. Step the PPU logic
        ppu.update(delta: delta)
        
        // 2. Update the SpriteNode with the latest FrameBuffer
        let texture = SKTexture(data: Data(ppu.frameBuffer), size: size)
        texture.filteringMode = .nearest
        screenNode.texture = texture
        screenNode.size = CGSize(width: ppu.width, height: ppu.height)
    }
}
