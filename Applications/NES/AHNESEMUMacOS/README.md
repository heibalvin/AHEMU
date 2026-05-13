# AH NES Emulator - macOS Application

## Architecture Overview

This macOS application implements an NES emulator using SwiftUI and our custom `NESEMU` core library. The UI layer consists of two main views: `NESEMUView` (the main container) and `NESPPUView` (the PPU rendering view).

## NESEMUView & NESEMUHolder

### NESEMUView

`NESEMUView` is the primary SwiftUI view that composes the emulator UI. It displays the PPU rendering view and a status bar showing UPS (Updates Per Second). It uses a `@StateObject` wrapper around `NESEMUHolder` to manage the emulator's lifetime.

### NESEMUHolder

`NESEMUHolder` is an `ObservableObject` that owns and manages the `NESEMU` instance and its execution thread. It does **not** own the PPU rendering — that is delegated to `NESPPUView` which receives a reference to the same `NESEMU` instance.

#### Thread Model

The emulator runs on a dedicated background thread to maximize performance and keep the UI responsive. The `start()` method creates a thread that continuously calls `emu.update(currentTime)` as fast as possible. Each update call computes a delta time and executes the appropriate number of CPU cycles.

```swift
thread = Thread { [weak self] in
    guard let self else { return }
    while self.running {
        let currentTime = CFAbsoluteTimeGetCurrent()
        self.emu.update(currentTime)
        self.updateCount += 1  // Thread-safe via NSLock
    }
}
```

The `stop()` method sets `running = false`, causing the thread loop to exit.

#### UPS Calculation

UPS measures how many times `emu.update()` is called per second. This is distinct from CPU cycle throughput:

- Each `emu.update()` call advances the emulator by a wall-clock delta and runs approximately `deltaTime * clock` CPU cycles (where `clock = masterClock / 4 = ~5.37 MHz`)
- By counting `update()` calls, we measure how frequently the emulator core is ticking
- A timer fires every 1.0 second on the main thread, reads and resets `updateCount` (protected by `NSLock`), then publishes: `ups = Double(updateCount) / deltaTime`
- The view displays this as an integer (rounded via `specifier: "%.0f"`)

**Example:** at 6.67 KHz UPS, the emulator thread calls `update()` every ~0.150 ms. Each call runs ~803 CPU cycles (for 60 FPS) or ~2333 CPU cycles (for 30 FPS).

## NESPPUView & NESRenderScene

### NESPPUView

`NESPPUView` is a SwiftUI view that renders the NES PPU output. It accepts an `NESEMU` instance in its initializer, creates its own `@StateObject` `NESRenderScene`, and manages the scene's FPS timer lifecycle.

### NESRenderScene

`NESRenderScene` is an `SKScene` (SpriteKit) that renders the NES frame buffer to the screen. It is an `ObservableObject` exposing a `@Published var fps: Double` for SwiftUI to display.

#### Frame Buffer Update

The PPU writes pixel data to an internal frame buffer as it scans each dot/line of the 256×240 resolution. When a full frame is complete, the internal buffer is swapped with the visible buffer under a lock (`NSLock`). The scene's `update(_:)` method:

1. Reads the current frame buffer via `ppu.getFrameBuffer()` (thread-safe)
2. Creates an `SKTexture` from the RGBA byte data
3. Assigns the texture to `screenNode`
4. Increments `frameCount` for FPS calculation

#### No Throttling

`NESRenderScene` renders **every** SpriteKit frame without throttling. FPS is measured (not enforced). The display's native refresh rate (typically 60 Hz on modern macOS displays) determines the actual rendered FPS.

#### FPS Calculation

A cancellable Combine `Timer` fires every 1.0 second on the main thread:

```swift
fps = Double(frameCount) / deltaTime
frameCount = 0
```

`frameCount` is incremented in `update(_:)` on each SpriteKit frame render callback.

**Target:** 60 FPS (NTSC NES standard), ~16.67 ms per frame.  
The NES PPU naturally produces one complete frame every ~16.67 ms at 60 Hz.

### Relationship Between UPS and FPS

- **UPS** measures emulator core update calls — how often the NES CPU/PPU state is advanced
- **FPS** measures display renders — how often the screen image is refreshed

For accurate emulation at 60 FPS, the emulator must process a full NES frame (~29780 CPU cycles) within 16.67 ms. This requires UPS of at least:

```
required UPS = (cycles per frame) / (frame time)
             ≈ 29780 / 0.01667
             ≈ 1.79 MHz
```

Our `NESEMUHolder` target (6.67 KHz) is **far below** this — meaning the emulator currently runs much slower than real-time. This is expected during early development or due to host system performance.

The `cycleId` in `NESEMU` is an internal counter of total CPU cycles executed across all `update()` calls. It is **not** used for UPS display, which instead measures call frequency.

---

## Technical Notes

- **Thread Safety:** `NESPPU` uses `NSLock` to protect the double-buffered frame buffer swap. `NESEMUHolder` uses `NSLock` for the `updateCount`.
- **Memory:** The frame buffer is `256 × 240 × 4` bytes (RGBA) ≈ 245 KB.
- **Timer Granularity:** Combine `Timer` runs on the main run loop; suitable for display metrics but not precise timing.
- **ObservableObject:** `NESRenderScene` conforms to `ObservableObject` only for its `@Published fps` property.
- **SwiftUI Lifecycle:** `startFPSTimer()` / `stopFPSTimer()` are called in `.onAppear` / `.onDisappear` of `NESPPUView`.
