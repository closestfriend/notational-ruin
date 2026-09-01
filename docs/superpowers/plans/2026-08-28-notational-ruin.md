# Notational Ruin Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a dependency-free C++20 CLI that creates a clean procedural score SVG and a materially degraded monochrome PNG.

**Architecture:** A shared `Score` model feeds SVG and raster renderers. The raster output passes through explicit image-processing functions and a repeated xerox loop before a tiny built-in PNG encoder writes the result.

**Tech Stack:** C++20, CMake, Apple Clang, standard library only, assert-based C++ test executable.

**Spec:** `docs/superpowers/specs/2026-08-28-notational-ruin-design.md`

## Global Constraints

- Use C++20 and compile cleanly with Apple Clang on macOS.
- Keep the project dependency-free; do not shell out to image tools.
- Keep generation and degradation as visibly distinct stages.
- Use a seeded `std::mt19937` so the same seed and parameters reproduce the same files.
- Prefer structs, vectors, references, value semantics, and small free functions over elaborate abstractions.
- Provide no GUI.

### Task 1: Project skeleton and image primitives

**Files:**
- Create: `CMakeLists.txt`
- Create: `src/image.hpp`
- Create: `src/image.cpp`
- Create: `tests/test_image.cpp`

**Interfaces:**
- Produce `struct Image { int width; int height; std::vector<uint8_t> pixels; }` with `at(x,y)` accessors and `fill()`.
- Produce `Image resizeNearest(const Image&, int, int)`.

- [ ] Write failing tests asserting a 2×2 image resizes to four repeated quadrants and out-of-range dimensions are rejected.
- [ ] Run `cmake -S . -B build && cmake --build build --target notational-ruin-tests`; expect failure because image symbols do not exist.
- [ ] Implement the image value type and nearest-neighbor resize with direct nested loops.
- [ ] Re-run the focused test and confirm it passes.
- [ ] Commit with `git add CMakeLists.txt src tests && git commit -m "build: add image primitives"`.

### Task 2: Readable image-processing passes

**Files:**
- Create: `src/image_processing.hpp`
- Create: `src/image_processing.cpp`
- Modify: `tests/test_image.cpp`

**Interfaces:**
- Produce `blurImage`, `addNoise`, `thresholdImage`, `adaptiveThreshold`, `erodeImage`, `dilateImage`, and `xeroxGeneration` as value-returning functions.
- `thresholdImage` accepts normalized threshold/noise inputs and returns pixels only 0 or 255.

- [ ] Add failing assertions for blur spreading a dark pixel, hard threshold producing binary values, erosion removing an isolated pixel, dilation expanding one, and xerox depth changing output.
- [ ] Run the focused test and observe the expected failures.
- [ ] Implement box blur, deterministic hash-based noise, thresholding, spatial sine/hash threshold variation, 3×3 binary morphology, and one xerox generation.
- [ ] Re-run tests until all image-processing assertions pass.
- [ ] Commit with `git add src/image_processing.* tests/test_image.cpp && git commit -m "feat: add destructive image pipeline"`.

### Task 3: Score data model and deterministic generator

**Files:**
- Create: `src/random.hpp`
- Create: `src/score.hpp`
- Create: `src/score.cpp`
- Create: `tests/test_score.cpp`

**Interfaces:**
- Produce `ScoreParameters`, `Point`, `Stroke`, `TextMark`, `Staff`, and `Score` structs.
- Produce `Score generateScore(const ScoreParameters&)`.
- Keep generator helpers named `generateStaff`, `generateNoteCluster`, `generateAnnotation`, `generateConnector`, and `generateOversizedMark`.

- [ ] Add failing tests that generate twice with the same parameters and compare serialized model data, and assert 3–8 staffs plus non-empty marks.
- [ ] Run the focused test and observe failure.
- [ ] Implement seeded generation of irregular staffs, notation marks, annotations, connectors, collision-prone clusters, negative-space-aware regions, and oversized marks; put 12–15 `TINKER HERE` comments near probability/distortion decisions.
- [ ] Re-run the score tests and confirm deterministic, bounded output.
- [ ] Commit with `git add src/random.hpp src/score.* tests/test_score.cpp && git commit -m "feat: generate deterministic graphic scores"`.

### Task 4: SVG and bitmap renderers

**Files:**
- Create: `src/svg_renderer.hpp`
- Create: `src/svg_renderer.cpp`
- Create: `src/raster_renderer.hpp`
- Create: `src/raster_renderer.cpp`
- Modify: `tests/test_score.cpp`

**Interfaces:**
- Produce `std::string renderSvg(const Score&)` and `Image renderRaster(const Score&)`.
- SVG helpers cover line, circle, ellipse, path, text, and rect; raster helpers draw the same primitive vocabulary in black on white.

- [ ] Add failing tests for SVG dimensions/element presence and raster dimensions/non-white pixels.
- [ ] Run the test and confirm failure.
- [ ] Implement direct SVG serialization and simple raster line/circle/rect/text approximations.
- [ ] Re-run tests and confirm both renderers produce visible output.
- [ ] Commit with `git add src/*renderer.* tests/test_score.cpp && git commit -m "feat: render clean scores to svg and bitmap"`.

### Task 5: PNG output and command-line interface

**Files:**
- Create: `src/png_writer.hpp`
- Create: `src/png_writer.cpp`
- Create: `src/main.cpp`
- Modify: `CMakeLists.txt`
- Create: `tests/test_cli.cpp`

**Interfaces:**
- Produce `bool writePng(const Image&, const std::string&)`.
- Parse `ruin` or a prefix plus all documented technical flags, validate dimensions and normalized parameters, and write `<prefix>_clean.svg` and `<prefix>_ruined.png`.

- [ ] Add a failing CLI test invoking a tiny image write and checking the eight-byte PNG signature and expected output paths.
- [ ] Run the test and confirm failure.
- [ ] Implement CRC/adler helpers and PNG chunks using uncompressed DEFLATE blocks; implement CLI defaults, help, errors, stage ordering, and output naming.
- [ ] Re-run the CLI test and confirm valid files are emitted.
- [ ] Commit with `git add src/main.cpp src/png_writer.* CMakeLists.txt tests/test_cli.cpp && git commit -m "feat: add cli and png export"`.

### Task 6: README and end-to-end verification

**Files:**
- Create: `README.md`
- Modify: `CMakeLists.txt` if needed for test registration.

- [ ] Add README build/run commands, full option table, stage explanation, pipeline order, blur/threshold relationship, erosion/dilation, downsampling, xerox depth, and 10 parameter recipes.
- [ ] Build from a clean build directory and run the complete test executable.
- [ ] Run `./build/notational-ruin ruin --seed 18 --width 640 --height 400` and inspect file signatures, dimensions, and deterministic hashes across a repeated run.
- [ ] Run contrasting commands for threshold variation, erosion, downsampling, and xerox depth; confirm output hashes differ.
- [ ] Run `git diff --check`, review the requirement checklist against the spec, and commit with `git add README.md && git commit -m "docs: explain notational ruin"`.
