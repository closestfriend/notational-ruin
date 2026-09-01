# Notational Ruin Design

## Goal

Build a small C++20 command-line program that generates a deterministic, strange graphic score as a clean SVG and then materially transforms the same score through a readable monochrome reproduction pipeline into a PNG.

## Constraints

- Use C++20 and compile cleanly with Apple Clang on macOS.
- Keep the project dependency-free; do not shell out to image tools.
- Keep generation and degradation as visibly distinct stages.
- Use a seeded `std::mt19937` so the same seed and parameters reproduce the same files.
- Prefer structs, vectors, references, value semantics, and small free functions over elaborate abstractions.
- Provide no GUI.

## Architecture

The generator produces a shared `Score` value containing staffs and graphic marks. The SVG renderer serializes the score with direct XML helpers. The raster renderer interprets the same marks into an 8-bit grayscale `Image`, avoiding an SVG rasterization dependency.

The image pipeline operates on `Image` values: optional nearest-neighbor reduction, box blur, deterministic luminance noise, hard or spatially varying threshold, binary erosion/dilation, and repeated xerox generations. A tiny PNG encoder writes the final grayscale image as an RGB PNG using uncompressed DEFLATE blocks, so the build requires no third-party library.

## CLI

`notational-ruin ruin [options]` writes `<prefix>_clean.svg` and `<prefix>_ruined.png`. The default prefix is `output`; the first positional argument is also accepted as a prefix for compatibility with `notational-ruin output --seed 1234`.

Supported options: `--seed`, `--width`, `--height`, `--density`, `--chaos`, `--threshold`, `--threshold-variation`, `--blur`, `--noise`, `--erosion`, `--dilation`, `--downsample`, and `--xerox-depth`.

## Generation

Generate 3–8 irregular staff systems with bent/drifting/broken lines. Add on-staff and off-staff note clusters, stems, occasional beams, rests, fermata-like arcs, articulation dots, event lines, brackets, boxes, arrows, circles, connectors, rehearsal marks, numbers, durations, sparse instructions, collisions, malformed marks, negative space, dense pockets, and oversized symbols. Higher-level parameters map to explicit probabilities so beginner experiments are easy to find.

## Degradation

The first generation uses `downsample` as a scale factor, then blur, noise, threshold, erosion, and dilation. Threshold compares normalized luminance against the exposed cutoff; adaptive mode adds deterministic low-frequency sine variation and hash noise. Each xerox generation repeats blur, a tiny deterministic horizontal drift, threshold, and noise, accumulating loss. The result is upscaled with nearest-neighbor sampling to the requested output dimensions.

## Testing and documentation

Tests use a small C++ executable with assertions and cover deterministic score generation, image operations, PNG signature/output, and CLI generation. README explains macOS build/run instructions, pipeline ordering, threshold/blur interaction, erosion/dilation, downsampling, xerox depth, tinker points, and visibly different recipes.
