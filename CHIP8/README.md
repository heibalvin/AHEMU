# CHIP-8 Emulator

## Prerequisites

- **g++** (C++17-capable)
- **SDL3** (via Homebrew: `brew install sdl3`)
- `make`

## Compile

```bash
make
```

The binary is produced at `./Build/chip8emu`.

## Run

```bash
make run                    # builds & runs with the default ROM
./Build/chip8emu <rom.ch8>  # run a specific ROM file
```

## Other Targets

| Command | Description |
|---|---|
| `make clean` | Remove all build artefacts |
| `make lint` | Run `cppcheck` static analysis |
| `make typecheck` | Run `clang-tidy` type checking |

## SDLEMU Class

The `SDLEMU` class provides the SDL3 interface for the CHIP-8 emulator:

- **Window**: 1280 x 640 pixels
- **Texture**: 64 x 32 pixels (ARGB8888 format) with `SDL_LOGICAL_PRESENTATION_LETTERBOX`
- **Splash Screen**: Displays splash-screen.jpeg from project root directory

### Class Structure

```cpp
class SDLEMU {
private:
    SDL_Window*   window;      // 1280x640 window
    SDL_Renderer* renderer;    // SDL renderer
    SDL_Texture*  texture;     // 64x32 ARGB texture
    SDL_Texture*  splashTexture;
    bool          running;

    char projectPath[256];     // Base path (../ + executable dir)
    char resourcePath[256];    // Resources subdirectory path
    Uint64 fpsTarget;          // Target FPS (60 by default)
};
```

### Methods

| Method | Description |
|---|---|
| `SDLInit()` | Initialize SDL, create window/renderer/texture. Called in constructor. |
| `SDLRelease()` | Cleanup SDL resources. Called in destructor. |
| `getProjectPath()` | Find parent path with .git folder, append CHIP8. |
| `start()` | Load splash screen from projectPath, initialize timing. |
| `stop()` | Stop the emulator loop. |
| `run()` | Main game loop with event handling and rendering. |
| `update(deltaTime)` | Update emulator state. |
| `render(deltaTime)` | Render frame at `fpsTarget` Hz (default 60). |
| `getFilePath(filename, path)` | Build file path from projectPath with optional subpath. |

## References

- [CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite) — ROM tests for verifying emulator correctness.
- [CHIP-8 Emulator (C++)](https://austinmorlan.com/posts/chip8_emulator/) — A helpful guide for building a CHIP-8 emulator in C++.

The emulator maps the original CHIP-8 16-key keypad to the keyboard as follows:

| CHIP-8 key | Host key |
|---|---|
| 1 | 1 |
| 2 | 2 |
| 3 | 3 |
| C | C |
| 4 | 4 |
| 5 | 5 |
| 6 | 6 |
| D | D |
| 7 | 7 |
| 8 | 8 |
| 9 | 9 |
| E | E |
| A | A |
| 0 | 0 |
| B | B |
| F | F |
