# SDL3 NESPPU Demo

SDL3 C++ demo rendering color cycling frames using NESPPU double-buffering.

## Architecture

- **NESPPU** (Picture Processing Unit): maintains two RGBA32 buffers (`frameBuffer` and `internalFrameBuffer`). `step()` writes pixels to `internalFrameBuffer` (256×240). On frame completion, buffers swap and `isRefreshReq` is set.
- **NESEMU**: forwards `step()` to PPU and counts step calls per `update()`. Provides `getStepCount()` and `resetStepCount()` for metrics.
- **main.cpp**: calls `emu->update()` each loop, checks `isRefreshRequested()`, updates texture from `frameBuffer`, and draws a debug text overlay showing UPS (updates per second in MHz) and step count per frame.

## Build

```bash
make
```

## Run

```bash
make run
```

## Performance

Separate FPS and UPS overlay:

- **FPS** (Frames Per Second): rate at which `isRefreshRequested()` triggers (i.e., frames actually rendered to screen)
- **UPS** (Updates Per Second): rate at which `emu->update()` is called in the main loop

Typical measured values: FPS ~60 (display refresh rate), UPS ~0.48 MHz (~480,000 updates/sec). Each update corresponds to ~11 PPU clock cycles at the NES's ~5.37 MHz CPU/PPU clock.