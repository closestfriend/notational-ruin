cmake_minimum_required(VERSION 3.20)
project(notational_ruin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(notational_ruin_core
    src/image.cpp
    src/image_processing.cpp
    src/score.cpp
    src/svg_renderer.cpp
    src/raster_renderer.cpp
    src/png_writer.cpp
)
target_include_directories(notational_ruin_core PUBLIC src)
target_compile_options(notational_ruin_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(notational-ruin src/main.cpp)
target_link_libraries(notational-ruin PRIVATE notational_ruin_core)

add_executable(notational-ruin-tests
    tests/test_image.cpp
    tests/test_score.cpp
    tests/test_cli.cpp
)
target_link_libraries(notational-ruin-tests PRIVATE notational_ruin_core)
target_compile_options(notational-ruin-tests PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_test(NAME notational-ruin-tests COMMAND notational-ruin-tests)

if(APPLE)
    add_executable(notational-ruin-gui MACOSX_BUNDLE src/macos_gui.mm)
    set_source_files_properties(src/macos_gui.mm PROPERTIES COMPILE_FLAGS "-fobjc-arc")
    target_link_libraries(notational-ruin-gui PRIVATE notational_ruin_core "-framework Cocoa" "-framework UniformTypeIdentifiers")
    target_compile_features(notational-ruin-gui PRIVATE cxx_std_20)
endif()
