import Foundation

public final class NESPPU {
    // Components
    public var emu: NESEMU

    // Screen Resolution and Buffer
    public let width: Int = 256
    public let height: Int = 240
    public var frameBuffer: [UInt8] = []
    
    // PPU specific variables
    private var dot = 0
    private var line = 0
    private var colour = 0

    public init(emu: NESEMU) {
        self.emu = emu
        self.frameBuffer = Array(repeating: 0x00, count: width * height * 4) // RGBA format with 4 bytes per pixel
    }
    
    public func update(delta: TimeInterval) {
        let cyclesPerSecond = emu.clock
        let cyclesToRun = Int(delta * cyclesPerSecond)
        for _ in 0..<cyclesToRun {
            step()
        }
        print("NESPPU: updated \(cyclesToRun) cycles")
    }

    public func step() {
        let baseIndex = (line * width + dot) * 4
        frameBuffer[baseIndex] = UInt8(colour)        // R
        frameBuffer[baseIndex + 1] = UInt8(colour)    // G
        frameBuffer[baseIndex + 2] = UInt8(colour)      // B
        frameBuffer[baseIndex + 3] = 0xFF               // A
        
        dot += 1
        if dot >= width {
            dot = 0
            line += 1
            if line >= height {
                dot = 0
                line = 0
                colour = (colour + 1) % 256
            }
        }
    }
}
