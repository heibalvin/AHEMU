# AHEMU

Emulator & Tools for Retro Consoles (NES, SMS, GB, PCE, ...)

## Strategy Change

The initial strategy was to build a NESEMU (NES Emulator) on **SwiftUI with Swift Package**. We are now testing an alternative approach using **SDL3 / C++ with Dear ImGui**, as the SwiftUI approach is ~800× slower than expected for real-time emulation.

### SwiftUI / Swift Approach (SwiftUINESEMU)

The SwiftUI-based emulator is available in [`SwiftUINESEMU/`](SwiftUINESEMU/).

| Metric | Measure | Target | Status |
|--------|---------|--------|--------|
| **FPS (Frame Per Second)** | 60 FPS (~16.67 ms/frame) | 60 FPS (NTSC NES) | ✅ Achieved — SwiftUI renders at display refresh rate |
| **UPS (Update Per Second)** | 6.67 KHz (~0.150 ms/update) | 5.37 MHz (~0.000186 ms/update) | ❌ ~800× slower than required |

**Key takeaway:** SwiftUI updates the rendering at 60 Hz FPS (~16.67 ms per frame), which matches the NES NTSC target FPS. However, SwiftUI updates NESEMU on a separate thread at only 6.67 KHz UPS (~0.150 ms per update), which is far slower than the required 5.37 MHz (~0.000186 ms). As a result, the NESEMU needs to process approximately **806 CPU steps on every update** to keep up — far too many cycles per tick for real-time performance.

### SDL3 / C++ / Dear ImGui Approach (In Testing)

| Metric | Target |
|--------|--------|
| **FPS** | 60 FPS (~16.67 ms/frame) |
| **UPS** | 5.37 MHz (~0.000186 ms/update) |
| **CPU Steps per Update** | ~29,780 cycles per frame, spread across timely ticks |

The SDL3 / C++ / Dear ImGui approach is being evaluated to achieve the required UPS and real-time emulation performance that SwiftUI cannot currently deliver.