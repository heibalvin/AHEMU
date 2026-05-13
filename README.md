# AHEMU

Emulator & Tools for Retro Consoles (NES, SMS, GB, PCE, ...)

## Strategy Change

The initial strategy was to build a NESEMU (NES Emulator) on **SwiftUI with Swift Package**. We are now testing an alternative approach using **SDL3 / C++ with Dear ImGui**, as the SwiftUI approach is ~800× slower than expected for real-time emulation.

## Performance Comparison

| Implementation | FPS | UPS | Update Time | CPU Steps/Update | Status |
|----------------|-----|-----|-------------|------------------|--------|
| **SwiftUI / Swift** | 60 FPS | 6.67 KHz | 0.150 ms | ~806 | ❌ ~800× slower than target |
| **SDL3 / C++** | 60 FPS | 0.316 MHz | 0.00316 ms | ~17 | ❌ ~17× slower than target |

**Target:** 5.37 MHz (~0.000186 ms/update)

### SwiftUI / Swift Approach (SwiftUINESEMU)

The SwiftUI-based emulator is available in [`SwiftUINESEMU/`](SwiftUINESEMU/).

| Metric | Measure | Target | Status |
|--------|---------|--------|--------|
| **FPS (Frame Per Second)** | 60 FPS (~16.67 ms/frame) | 60 FPS (NTSC NES) | ✅ Achieved — SwiftUI renders at display refresh rate |
| **UPS (Update Per Second)** | 6.67 KHz (~0.150 ms/update) | 5.37 MHz (~0.000186 ms/update) | ❌ ~800× slower than required |

**Key takeaway:** SwiftUI updates the rendering at 60 Hz FPS (~16.67 ms per frame), which matches the NES NTSC target FPS. However, SwiftUI updates NESEMU on a separate thread at only 6.67 KHz UPS (~0.150 ms per update), which is far slower than the required 5.37 MHz (~0.000186 ms). As a result, the NESEMU needs to process approximately **806 CPU steps on every update** to keep up — far too many cycles per tick for real-time performance.

### SDL3 / C++ / Dear ImGui Approach (In Testing)

| Metric | Measure | Target | Status |
|--------|---------|--------|--------|
| **FPS** | 60 FPS (~16.67 ms/frame) | 60 FPS (NTSC NES) | ✅ Achieved |
| **UPS** | 0.316 MHz (~0.00316 ms/update) | 5.37 MHz (~0.000186 ms/update) | ❌ ~17× slower than required |
| **CPU Steps per Update** | ~17 cycles | ~17 cycles target | ⚠️ Near target per update |

**Key takeaway:** SDL3 / C++ runs at 0.316 MHz with ~17 cycles per update. However, it still falls ~17× short of the 5.37 MHz target needed for real-time NES emulation.