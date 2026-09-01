# Notational Ruin

Notational Ruin is a small C++20 command-line prototype that makes a strange graphic score, exports the clean geometry as SVG, and then reproduces the same marks until they become a severe black-and-white PNG.

## Build on macOS

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The project uses only the C++ standard library. Apple Clang 17 and CMake 3.20+ are sufficient.

## Native macOS GUI

On macOS, CMake also creates a small native AppKit application:

```sh
cmake --build build --target notational-ruin-gui
open build/notational-ruin-gui.app
```

The window exposes seed, density, chaos, threshold, blur, downsample, and xerox depth. `Generate Ruin` writes temporary clean SVG/ruined PNG files and updates the preview. The GUI is intentionally macOS-only and keeps the command-line program available for scripting.

## Run

```sh
./build/notational-ruin ruin --seed 18
```

This writes `output_clean.svg` and `output_ruined.png`. A custom prefix can be used as the first argument:

```sh
./build/notational-ruin score --seed 1234 --width 1600 --height 1000
```

Options include `--density`, `--chaos`, `--threshold`, `--threshold-variation`, `--blur`, `--noise`, `--erosion`, `--dilation`, `--downsample`, and `--xerox-depth`.

## Two stages

The clean stage creates 3–8 irregular five-line systems, drifting and broken staff lines, noteheads, stems, dots, rests, arcs, boxes, brackets, arrows, circles, connectors, numbers, durations, instructions, collisions, and occasional oversized marks. The same seeded model is rendered as both SVG and a clean bitmap.

The ruin stage follows this order:

`raster → downsample → blur → noise → threshold → erosion → dilation → xerox generations → nearest-neighbor upscale → PNG`

Blur matters before threshold: gray edges become hard black or white regions, so nearby lines fuse, thin stems vanish, and noteheads can join staffs. Threshold is normalized from 0 to 1. `--threshold-variation` adds deterministic sine-wave and low-frequency hash variation, making some regions heavier than others.

Erosion removes black pixels when any neighboring pixel is white, eating thin marks. Dilation adds black pixels when any neighbor is black, spreading ink and merging nearby marks. Downsampling discards fine geometry before the hard decisions and the nearest-neighbor upscale preserves chunky stair-step edges. `--xerox-depth 0` stops after the first pipeline; larger values blur, resample through hard threshold, and add noise again so loss accumulates.

## Beginner tinker points

Search the source for `TINKER HERE`. Useful first experiments are staff line breaks and distortion in `score.cpp`, note/stem ranges in `generateNoteCluster`, threshold wave frequency in `image_processing.cpp`, and generation blur in `main.cpp`.

## Recipes

```sh
./build/notational-ruin ruin --seed 18
./build/notational-ruin ruin --seed 18 --threshold 0.62 --blur 2.0
./build/notational-ruin ruin --seed 18 --xerox-depth 5 --downsample 0.35
./build/notational-ruin ruin --seed 18 --noise 0.12 --erosion 1
./build/notational-ruin ruin --seed 18 --threshold-variation 0.2
./build/notational-ruin ruin --seed 77 --density 0.3 --chaos 0.8 --xerox-depth 1
./build/notational-ruin ruin --seed 77 --density 1.0 --dilation 1 --blur 2.8
./build/notational-ruin ruin --seed 404 --downsample 0.18 --threshold 0.45 --xerox-depth 4
./build/notational-ruin ruin --seed 9 --noise 0.25 --threshold-variation 0.3 --erosion 1
./build/notational-ruin ruin --seed 9001 --density 0.5 --chaos 0.15 --blur 0.4 --xerox-depth 0
```
