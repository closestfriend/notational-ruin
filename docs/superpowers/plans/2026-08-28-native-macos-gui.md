# Native macOS GUI Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a tiny native AppKit window for adjusting core parameters and previewing the generated ruined PNG.

**Architecture:** Keep `notational_ruin_core` as the reusable C++ library. Add one Objective-C++ AppKit executable that owns controls, calls the existing C++ generation/rendering functions, writes temporary SVG/PNG files, and reloads the PNG into an `NSImageView`.

**Tech Stack:** C++20, Objective-C++, AppKit/Cocoa, CMake, standard library only.

**Spec:** `docs/superpowers/specs/2026-08-28-notational-ruin-design.md`

## Global Constraints

- The GUI is macOS-specific and must be conditionally configured with `if(APPLE)`.
- The existing CLI target and test target must remain available.
- No third-party GUI dependencies or external image tools.
- GUI generation runs on the main thread for this deliberately small prototype.
- Temporary preview files go under the system temporary directory.

### Task 1: AppKit window and controls

**Files:**
- Create: `src/macos_gui.mm`
- Modify: `CMakeLists.txt`
- Modify: `README.md`

- [ ] Add an `if(APPLE)` CMake executable named `notational-ruin-gui`, compile as Objective-C++, link `notational_ruin_core` and Cocoa, and set C++20.
- [ ] Implement a controller with seed field, sliders for density/chaos/threshold/blur/downsample/xerox depth, Generate button, status label, and image preview.
- [ ] Connect Generate to `generateScore`, `renderSvg`, `renderRaster`, the same degradation sequence, and `writePng`; load the result into `NSImageView`.
- [ ] Add README macOS launch instructions and a note that the GUI is intentionally macOS-only.

### Task 2: Verification

**Files:**
- Modify: `CMakeLists.txt` only if target wiring needs correction.

- [ ] Build the existing CLI and test targets and run `ctest --test-dir build --output-on-failure`.
- [ ] Configure/build the GUI target on macOS and confirm the executable links against AppKit.
- [ ] Run the GUI executable as a smoke check that the process launches without an immediate exception; visually inspect the window when available.
