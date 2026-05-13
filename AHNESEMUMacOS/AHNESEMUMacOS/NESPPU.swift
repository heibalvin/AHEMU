import Foundation

public final class NESPPU {
    // Components
    public var emu: NESEMU

    // Screen Resolution and Buffer
    public let width: Int = 256
    public let height: Int = 240
    public var frameBuffer: [UInt8] = []
    var internalFrameBuffer: [UInt8] = []
    private let bufferLock = NSLock()

    // PPU specific variables
    private var dot = 0
    private var line = 0
    private var colour = 0

    public init(emu: NESEMU) {
        self.emu = emu
        self.frameBuffer = Array(repeating: 0x00, count: width * height * 4)
        self.internalFrameBuffer = Array(repeating: 0x00, count: width * height * 4)
    }

    public func step() {
        let baseIndex = (line * width + dot) * 4
        internalFrameBuffer[baseIndex] = UInt8(colour)        // R
        internalFrameBuffer[baseIndex + 1] = UInt8(colour)    // G
        internalFrameBuffer[baseIndex + 2] = UInt8(colour)      // B
        internalFrameBuffer[baseIndex + 3] = 0xFF               // A

        dot += 1
        if dot >= width {
            dot = 0
            line += 1
            if line >= height {
                dot = 0
                line = 0
                colour = (colour + 1) % 256

                bufferLock.lock()
                swap(&frameBuffer, &internalFrameBuffer)
                bufferLock.unlock()
            }
        }
    }

    public func getFrameBuffer() -> [UInt8] {
        bufferLock.lock()
        defer { bufferLock.unlock() }
        return frameBuffer
    }
}