# ASCII Video Player

Transform any video into stunning **colored ASCII art** and play it directly in your terminal! This project features full 24-bit RGB truecolor support, real-time video processing, and customizable character sets for different visual styles.

![ASCII Video Player](https://img.shields.io/badge/C%2B%2B-11-blue) ![OpenCV](https://img.shields.io/badge/OpenCV-4.x-green) ![License](https://img.shields.io/badge/license-MIT-orange)

## Features

### 🎨 Full Truecolor Support (24-bit RGB)

Every pixel's true color is preserved and rendered using ANSI escape codes, creating vibrant and accurate colored ASCII art in your terminal.

### 🚀 Direct MP4 Playback

No pre-conversion needed! Play any video file directly:
- Supports: `.mp4`, `.avi`, `.mkv`, `.mov`
- Real-time frame conversion using OpenCV
- Configurable width and character sets on the fly

### 💾 Pre-conversion for Instant Playback

Convert videos to `.obj` files for repeated viewing:
- Saves all frames with embedded color codes
- Instant playback without re-processing
- JSON format for easy debugging

### 🎭 Multiple Character Sets

Choose from 4 carefully designed character sets:
- **`short`** (4 chars): ` .:|` - Clean and minimal
- **`medium`** (10 chars): ` .:-=+*#%@` - Balanced detail (default)
- **`long`** (70 chars): Full ASCII range - Maximum texture and detail
- **`blocks`** (5 chars): ` ░▒▓█` - Retro pixel-art aesthetic

### ⚡ Performance Optimizations

- Efficient screen clearing using ANSI escape codes
- Cursor hiding during playback for clean output
- Brightness-based character mapping
- Modular architecture with shared decode logic

## Installation

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/Jeyor1337/ASCII-Video-Player.git
    cd ASCII-Video-Player
    ```

2.  **Install dependencies:**

    **Required libraries:**
    - OpenCV 4.x (for video processing)
    - json-c (for .obj file format)

    On Debian/Ubuntu:
    ```bash
    sudo apt-get install libopencv-dev libjson-c-dev
    ```

    On macOS (with Homebrew):
    ```bash
    brew install opencv json-c
    ```

3.  **Compile the project:**
    ```bash
    make
    ```

    This will build two executables in the `build/` directory:
    - `build/translator` - Converts videos to .obj format
    - `build/interpreter` - Plays videos (MP4 or .obj files)

## Usage

The project supports two modes: **direct playback** and **pre-conversion**.

### Option 1: Direct Playback (Recommended)

Play video files directly without pre-conversion:

```bash
build/interpreter <video_file> [--width <w>] [--charset <name>]
```

**Arguments:**
*   `<video_file>`: Path to video file (.mp4, .avi, .mkv, .mov)
*   `--width <w>`: (Optional) ASCII width in columns. Defaults to 80.
*   `--charset <name>`: (Optional) Character set to use. Options: `short`, `medium` (default), `long`, `blocks`

**Examples:**
```bash
# Basic playback
build/interpreter miyu.mp4 --width 100

# With custom charset for maximum detail
build/interpreter miyu.mp4 --width 120 --charset long

# Block style for retro look
build/interpreter miyu.mp4 --width 80 --charset blocks
```

### Option 2: Pre-convert and Play

For repeated playback, you can pre-convert videos to `.obj` files.

**Step 1: Convert video**
```bash
build/translator <input_video> <output_obj> [--width <w>] [--charset <name>]
```

**Arguments:**
*   `<input_video>`: Path to the input video file
*   `<output_obj>`: Path for the output `.obj` file
*   `--width <w>`: (Optional) ASCII width. Defaults to 120.
*   `--charset <name>`: (Optional) Character set. Options: `short`, `medium` (default), `long`, `blocks`

**Step 2: Play converted file**
```bash
build/interpreter <output_obj>
```

**Note:** Pre-converted `.obj` files can be very large (e.g., 300MB+ for long videos) but allow instant playback without re-processing.

## Architecture

The project uses a modular architecture with shared components:

```
ASCII-Video-Player/
├── src/                # Source files
│   ├── ascii_decode.h  # Shared ASCII conversion logic
│   ├── translator.cpp  # Video → .obj converter
│   └── interpreter.cpp # Playback engine (supports MP4 & .obj)
├── build/              # Compiled executables
│   ├── translator
│   └── interpreter
└── Makefile            # Build configuration
```

### Key Components

**`ascii_decode.h`** - Shared header library containing:
- Character set definitions (`CharSets` namespace)
- Frame-to-ASCII conversion algorithms (`ASCIIDecode` namespace)
- Color calculation and ANSI escape code generation

**`translator`** - Pre-conversion tool:
- Reads video files using OpenCV
- Converts each frame to colored ASCII
- Saves to JSON format (.obj files) for instant playback

**`interpreter`** - Unified playback engine:
- **Direct mode**: Streams MP4/AVI/MKV files in real-time
- **Pre-converted mode**: Loads and plays .obj files
- Handles timing, cursor management, and screen clearing

## Technical Details

### How It Works

**Color Processing:**
1. Each video frame is processed in BGR color space (OpenCV default)
2. RGB values are preserved for ANSI truecolor codes
3. Brightness is calculated using: `0.299*R + 0.587*G + 0.114*B`
4. Character selection based on brightness mapping to chosen charset

**ANSI Color Codes:**
- Uses 24-bit truecolor: `\033[38;2;R;G;Bm`
- Reset codes after each line to prevent terminal corruption
- Compatible with most modern terminal emulators

**Performance:**
- Direct playback: Real-time frame conversion using OpenCV
- Pre-converted: Instant playback from cached JSON
- Efficient screen clearing using ANSI escape sequences
- Cursor hiding during playback for clean output

### Terminal Requirements

- ANSI truecolor support (24-bit color)
- Compatible terminals: iTerm2, GNOME Terminal, Konsole, Windows Terminal, Alacritty, Kitty
- Recommended: Dark terminal background for better contrast

## Examples

```bash
# Quick playback of a video
build/interpreter video.mp4

# High-detail playback with custom width
build/interpreter video.mp4 --width 150 --charset long

# Retro block style
build/interpreter video.mp4 --width 80 --charset blocks

# Pre-convert for repeated playback
build/translator long_video.mp4 cached.obj --width 120
build/interpreter cached.obj  # Instant playback!

# Different character sets comparison
build/interpreter video.mp4 --width 100 --charset short   # Minimal
build/interpreter video.mp4 --width 100 --charset medium  # Balanced
build/interpreter video.mp4 --width 100 --charset long    # Detailed
build/interpreter video.mp4 --width 100 --charset blocks  # Retro
```

## Tips & Tricks

- **Terminal size**: Wider terminals support larger `--width` values
- **Performance**: Use `short` charset for slower systems or smoother playback
- **Visual quality**: `long` charset provides maximum detail and texture
- **File size**: Pre-converted files with color can be 300MB+ for long videos
- **Best results**: Use dark terminal backgrounds for better contrast
- **Frame rate**: Higher FPS videos may stutter on slower systems in direct mode
- **Testing**: Start with a short video clip to test width and charset settings

## Character Set Reference

| Name | Characters | Count | Best For |
|------|-----------|-------|----------|
| `short` | ` .:\|` | 4 | Fast playback, clean look |
| `medium` | ` .:-=+*#%@` | 10 | General use, balanced detail |
| `long` | (Full ASCII range) | 70 | Maximum detail, artistic |
| `blocks` | ` ░▒▓█` | 5 | Retro pixel art style |

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Uses OpenCV for video processing
- Uses json-c for JSON handling
- Inspired by classic ASCII art players and terminal visualization projects
