import Foundation
import os

public final class NESEMU {
    private let logger = Logger(subsystem: "com.ahemu.nes", category: "emulator")

    // Clock & Timing
    public let masterClock: Double
    public let clock: Double
    var previousTime: TimeInterval = 0.0
    var deltaTime: TimeInterval = 0.0

    // Components
    public var ppu: NESPPU!

    // Internal cycle counter
    private(set) var cycleId: Int = 0

    public init() {
        self.masterClock = 21_477_272
        self.clock = masterClock / 4

        self.ppu = NESPPU(emu: self)
    }

    public func update(_ currentTime: TimeInterval) {
        if previousTime == 0.0 {
            previousTime = currentTime
            return
        }

        deltaTime = currentTime - previousTime

        let cyclesToRun = Int(deltaTime * clock)
        for _ in 0..<cyclesToRun {
            step()
        }

        cycleId += cyclesToRun
        previousTime = currentTime

        let deltaMs = deltaTime * 1000.0
        logger.debug("Ran \(cyclesToRun) cycles in \(String(format: "%.3f", deltaMs), privacy: .public) ms")
    }

    func step() {
        ppu.step()
    }
}
