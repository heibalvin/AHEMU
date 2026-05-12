import Foundation

public final class NESEMU {
    // EMU variables
    public let masterClock: Double
    public let clock: Double

    // Components
    public var ppu: NESPPU!

    public init() {
        self.masterClock = 21_477_272
        self.clock = masterClock / 4

        self.ppu = NESPPU(emu: self)
    }
}
